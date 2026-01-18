from __future__ import annotations

import ctypes
import os
import shutil
import subprocess
import sys
import zlib
from concurrent.futures import ThreadPoolExecutor, as_completed
from datetime import datetime
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Tuple


# ==========================================================
# CONSOLE LOGGING
# ==========================================================

def log(msg: str) -> None:
    ts = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    print(f"{ts} {msg}")


def has_flag(flag: str) -> bool:
    flag_l = flag.lower()
    return any(a.lower() == flag_l for a in sys.argv[1:])


def pause() -> None:
    try:
        input("Press ENTER to close...")
    except Exception:
        pass


# ==========================================================
# ADMIN BOOTSTRAP
# ==========================================================

def ensure_admin() -> None:
    if has_flag("--elevated"):
        return

    try:
        is_admin = bool(ctypes.windll.shell32.IsUserAnAdmin())
    except Exception:
        is_admin = False

    if is_admin:
        return

    new_args: List[str] = []
    for a in sys.argv[1:]:
        if a.lower() in ("--elevated",):
            continue
        new_args.append(a)

    new_args.append("--elevated")

    args = " ".join([f'"{a}"' for a in new_args])
    cmdline = f'"{Path(__file__).resolve()}" {args}'.strip()

    log("Not admin. Relaunching elevated...")
    rc = ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, cmdline, None, 1)

    if rc <= 32:
        log("ERROR: Elevation cancelled or failed.")
        raise SystemExit(1)

    raise SystemExit(0)


# ==========================================================
# PROCESS TERMINATION
# ==========================================================

def terminate_config_tool_if_running() -> None:
    """
    If MGSHDFix Config Tool.exe is running, terminate it.
    Best-effort; logs outcome but does not fail the script.
    """

    exe_names = [
        "MGSHDFix Config Tool.exe",
        "METAL GEAR SOLID2.exe",
        "METAL GEAR SOLID3.exe",
        "METAL GEAR.exe",
        "launcher.exe",
    ]

    for exe_name in exe_names:
        try:
            result = subprocess.run(
                ["taskkill", "/IM", exe_name, "/F"],
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                check=False,
            )

            out = (result.stdout or "").strip()

            if result.returncode == 0:
                log(f"[OK] Terminated: {exe_name}")
            else:
                if out:
                    log(f"[INFO] {exe_name}: {out}")
                else:
                    log(f"[INFO] {exe_name}: not running")

        except FileNotFoundError:
            log("[WARN] taskkill not found. Skipping process termination.")
            return
        except Exception as exc:
            log(f"[WARN] Failed terminating {exe_name}: {type(exc).__name__}: {exc}")


# ==========================================================
# CONFIG
# ==========================================================

ROOT_MGS2 = Path(r"C:\Users\cmkoo\OneDrive\Vortex\metalgearsolid2mc\mods")
ROOT_MG_MG2 = Path(r"C:\Users\cmkoo\OneDrive\Vortex\metalgearandmetalgear2mc\mods")
ROOT_MGS3 = Path(r"C:\Users\cmkoo\OneDrive\Vortex\metalgearsolid3mc\mods")

ROOTS = [ROOT_MGS2, ROOT_MG_MG2, ROOT_MGS3]

SRC_ASI = Path(r"C:\Development\Git\MGSHDFix\x64\Release\MGSHDFix.asi")
SRC_CFG = Path(r"C:\Development\Git\MGSHDFix\x64\Release\MGSHDFix Config Tool.exe")

TARGETS = [
    ("MGSHDFix.asi", SRC_ASI),
    ("MGSHDFix Config Tool.exe", SRC_CFG),
]

SETTINGS_SOURCE = Path(r"G:\Steam\steamapps\common\MGS2\plugins\MGSHDFix.settings")
SETTINGS_TARGETS = [
    Path(r"G:\Steam\steamapps\common\MGS3\plugins\MGSHDFix.settings"),
    Path(r"G:\Steam\steamapps\common\MG and MG2\plugins\MGSHDFix.settings"),
]

LOG_FILES = [
    "MGSHDFix_Game.log",
    "MGSHDFix_Launcher.log",
]


# ==========================================================
# CRC32 + DEPLOY
# ==========================================================

def crc32_file(path: Path, chunk_size: int = 1024 * 1024) -> int:
    crc = 0
    with path.open("rb") as f:
        while True:
            data = f.read(chunk_size)
            if not data:
                break
            crc = zlib.crc32(data, crc)
    return crc & 0xFFFFFFFF


def iter_two_levels(root: Path) -> Iterable[Path]:
    if not root.exists():
        return
    for level1 in root.iterdir():
        if level1.is_dir():
            for level2 in level1.iterdir():
                if level2.is_dir():
                    yield level2


def build_jobs() -> List[Tuple[Path, Path]]:
    jobs: List[Tuple[Path, Path]] = []
    for root in ROOTS:
        for folder in iter_two_levels(root):
            for dst_name, src in TARGETS:
                dst = folder / dst_name
                if dst.exists():
                    jobs.append((src, dst))
    return jobs


def maybe_copy_crc32(src: Path, dst: Path, src_crc: int) -> str:
    try:
        dst_crc = crc32_file(dst)
    except FileNotFoundError:
        return f"[SKIP] Missing destination: {dst}"

    if dst_crc == src_crc:
        return f"[OK] {dst}"

    shutil.copy2(src, dst)
    return f"[UPD] {dst} ({dst_crc:08X} -> {src_crc:08X})"


# ==========================================================
# SYMLINK HELPERS
# ==========================================================

def can_create_symlink_in_dir(dir_path: Path) -> bool:
    probe_target = dir_path / "__mgshdfix_symlink_probe_target.tmp"
    probe_link = dir_path / "__mgshdfix_symlink_probe_link.tmp"

    try:
        dir_path.mkdir(parents=True, exist_ok=True)

        if probe_link.exists() or probe_link.is_symlink():
            probe_link.unlink()
        if probe_target.exists():
            probe_target.unlink()

        probe_target.write_text("probe", encoding="utf-8")
        probe_link.symlink_to(probe_target)

        probe_link.unlink()
        probe_target.unlink()
        return True
    except OSError:
        try:
            if probe_link.exists() or probe_link.is_symlink():
                probe_link.unlink()
            if probe_target.exists():
                probe_target.unlink()
        except OSError:
            pass
        return False


def ensure_settings_link_or_copy(src: Path, dst: Path) -> str:
    if not src.exists():
        return f"[ERROR] Source settings missing: {src}"

    dst.parent.mkdir(parents=True, exist_ok=True)

    try:
        if dst.is_symlink():
            current = Path(os.readlink(dst)).resolve()
            if current == src.resolve():
                return f"[OK] Link correct: {dst}"
    except OSError:
        pass

    can_link = can_create_symlink_in_dir(dst.parent)

    if not can_link:
        if dst.exists() or dst.is_symlink():
            return f"[SKIP] No symlink privilege, leaving existing: {dst}"
        try:
            shutil.copy2(src, dst)
            return f"[COPY] No symlink privilege, copied: {dst}"
        except OSError as exc:
            return f"[ERROR] Copy fallback failed: {dst} ({exc})"

    try:
        if dst.exists() or dst.is_symlink():
            dst.unlink()
        dst.symlink_to(src)
        return f"[LINK] {dst} -> {src}"
    except OSError as exc:
        return f"[ERROR] Failed to create symlink: {dst} -> {src} ({exc})"


def resolve_symlink_target(path: Path) -> Optional[Path]:
    try:
        if not path.is_symlink():
            return None
        return Path(os.readlink(path)).resolve()
    except OSError:
        return None


def ensure_file_symlink(src_file: Path, dst_file: Path) -> str:
    if not src_file.exists():
        return f"[ERROR] Source log missing: {src_file}"

    dst_file.parent.mkdir(parents=True, exist_ok=True)

    existing_target = resolve_symlink_target(dst_file)
    if existing_target is not None and existing_target == src_file.resolve():
        return f"[OK] Link correct: {dst_file}"

    can_link = can_create_symlink_in_dir(dst_file.parent)
    if not can_link:
        return f"[SKIP] No symlink privilege, leaving existing: {dst_file}"

    try:
        if dst_file.exists() or dst_file.is_symlink():
            dst_file.unlink()
        dst_file.symlink_to(src_file)
        return f"[LINK] {dst_file} -> {src_file}"
    except OSError as exc:
        return f"[ERROR] Failed to create symlink: {dst_file} -> {src_file} ({exc})"


# ==========================================================
# VORTEX MOD PACKAGE DISCOVERY (by presence of plugins\MGSHDFix.asi)
# ==========================================================

def find_mod_packages(root: Path) -> List[Path]:
    r"""
    Returns mod package roots (level1) that have a level2 folder containing MGSHDFix.asi,
    which in practice is: <mod>\plugins\MGSHDFix.asi
    """
    out: Dict[str, Path] = {}
    if not root.exists():
        return []

    for level2 in iter_two_levels(root):
        asi = level2 / "MGSHDFix.asi"
        if not asi.exists():
            continue
        mod_root = level2.parent
        out[str(mod_root.resolve()).lower()] = mod_root

    return sorted(out.values(), key=lambda p: p.name.lower())


def pick_mgs2_source_mod(mgs2_mods: List[Path], desired_asi_crc: int) -> Optional[Path]:
    if not mgs2_mods:
        return None

    for mod_root in mgs2_mods:
        asi_path = mod_root / "plugins" / "MGSHDFix.asi"
        if not asi_path.exists():
            continue
        try:
            if crc32_file(asi_path) == desired_asi_crc:
                return mod_root
        except OSError:
            continue

    return mgs2_mods[0]


# ==========================================================
# LOG SYMLINKS (MGS2 chosen mod is source; link into all others)
# ==========================================================

def link_logs_from_mgs2_source(desired_asi_crc: int) -> None:
    mgs2_mods = find_mod_packages(ROOT_MGS2)
    log(f"[LOG] MGS2 mod packages found: {len(mgs2_mods)} (root: {ROOT_MGS2})")
    if not mgs2_mods:
        return

    src_mod_root = pick_mgs2_source_mod(mgs2_mods, desired_asi_crc)
    if src_mod_root is None:
        return

    src_logs_dir = src_mod_root / "logs"
    log(f"[LOG] Using MGS2 source mod: {src_mod_root}")
    log(f"[LOG] Source logs dir:      {src_logs_dir}")

    targets = [
        ("MG and MG2", ROOT_MG_MG2),
        ("MGS3", ROOT_MGS3),
    ]

    for label, root in targets:
        mods = find_mod_packages(root)
        log(f"[LOG] {label} mod packages found: {len(mods)} (root: {root})")
        if not mods:
            continue

        for mod_root in mods:
            dst_logs_dir = mod_root / "logs"
            for log_name in LOG_FILES:
                src_log = src_logs_dir / log_name
                dst_log = dst_logs_dir / log_name
                log(ensure_file_symlink(src_log, dst_log))


# ==========================================================
# MAIN
# ==========================================================

def main() -> int:
    log("=== update_local_game_files.py start ===")

    if os.environ.get("CI"):
        log("CI environment detected. Skipping update.")
        return 0

    log(f"Python: {sys.version}")
    log(f"Script: {Path(__file__).resolve()}")
    log(f"User:   {os.environ.get('USERNAME', '')}")
    log(f"CWD:    {Path.cwd().resolve()}")
    log(f"Args:   {' '.join(sys.argv[1:])}")

    log("Ensuring admin...")
    ensure_admin()
    log("Admin OK.")

    log("Checking for running MGSHDFix Config Tool.exe...")
    terminate_config_tool_if_running()


    log(f"Checking build outputs:\n  ASI: {SRC_ASI}\n  CFG: {SRC_CFG}")
    if not SRC_ASI.exists() or not SRC_CFG.exists():
        log("ERROR: Source build outputs missing.")
        return 1

    log("Precomputing source CRC32...")
    src_crc_map = {
        SRC_ASI: crc32_file(SRC_ASI),
        SRC_CFG: crc32_file(SRC_CFG),
    }
    log(f"SRC CRC32 ASI: {src_crc_map[SRC_ASI]:08X}")
    log(f"SRC CRC32 CFG: {src_crc_map[SRC_CFG]:08X}")

    jobs = build_jobs()
    log(f"Vortex deploy targets found: {len(jobs)}")
    if jobs:
        max_workers = min(32, (os.cpu_count() or 8) * 2)
        log(f"Processing {len(jobs)} targets with {max_workers} workers...")

        with ThreadPoolExecutor(max_workers=max_workers) as ex:
            futures = [
                ex.submit(maybe_copy_crc32, src, dst, src_crc_map[src])
                for (src, dst) in jobs
            ]
            for fut in as_completed(futures):
                log(fut.result())
    else:
        log("No targets found for Vortex mods (no matching files at expected depth).")

    log("Ensuring MGSHDFix.settings linkage...")
    for dst in SETTINGS_TARGETS:
        log(ensure_settings_link_or_copy(SETTINGS_SOURCE, dst))

    log("Linking MGSHDFix log files (MGS2 is source)...")
    link_logs_from_mgs2_source(src_crc_map[SRC_ASI])

    log("Done.")
    return 0


if __name__ == "__main__":
    rc = 1
    try:
        rc = main()
    except SystemExit as exc:
        code = exc.code if isinstance(exc.code, int) else 0
        log(f"SystemExit: {code}")
        rc = code
        raise
    except Exception as exc:
        log(f"FATAL: {type(exc).__name__}: {exc}")
        raise
    finally:
        log(f"=== exit code {rc} ===")

        if has_flag("--pause"):
            pause()

    raise SystemExit(rc)
