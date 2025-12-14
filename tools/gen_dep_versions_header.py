#!/usr/bin/env python3
"""
Generate a C header file from ESP-IDF dependencies.lock

Usage:
    python gen_versions_header.py dependencies.lock version_info.h
"""

import sys
import yaml
from datetime import datetime

def die(msg):
    print(f"Error: {msg}", file=sys.stderr)
    sys.exit(1)

def main():
    if len(sys.argv) != 3:
        die("Usage: gen_versions_header.py <dependencies.lock> <output.h>")

    lock_path = sys.argv[1]
    out_path = sys.argv[2]

    try:
        with open(lock_path, "r") as f:
            lock = yaml.safe_load(f)
    except Exception as e:
        die(f"Failed to read lock file: {e}")

    deps = lock.get("dependencies", {})

    def get_version(name, default="UNKNOWN"):
        return deps.get(name, {}).get("version", default)

    esp_matter_ver = get_version("espressif/esp_matter")
    idf_ver = get_version("idf")
    target = lock.get("target", "UNKNOWN")
    manifest_hash = lock.get("manifest_hash", "UNKNOWN")
    lock_version = lock.get("version", "UNKNOWN")
    generated_at = datetime.utcnow().isoformat() + "Z"

    header = f"""\
/*
 * AUTO-GENERATED FILE — DO NOT EDIT
 *
 * Source: {lock_path}
 * Generated: {generated_at}
 */

#pragma once

/* Target */
#define GEN_DEP_BUILD_TARGET "{target}"

/* ESP-IDF */
#define GEN_DEP_ESP_IDF_VERSION "{idf_ver}"

/* esp-matter */
#define GEN_DEP_ESP_MATTER_VERSION "{esp_matter_ver}"

/* dependencies.lock metadata */
#define DEPENDENCIES_LOCK_VERSION "{lock_version}"
#define DEPENDENCIES_MANIFEST_HASH "{manifest_hash}"
"""

    try:
        with open(out_path, "w") as f:
            f.write(header)
    except Exception as e:
        die(f"Failed to write header file: {e}")

    print(f"Generated {out_path}")
    print(f"  Target       : {target}")
    print(f"  ESP-IDF      : {idf_ver}")
    print(f"  esp-matter   : {esp_matter_ver}")

if __name__ == "__main__":
    main()
