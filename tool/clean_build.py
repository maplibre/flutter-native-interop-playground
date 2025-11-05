#!/usr/bin/env python3

import subprocess
import shlex
import pathlib
from tqdm.auto import tqdm

plugin_root = pathlib.Path(__file__).parent.parent
maplibre_root = plugin_root / 'third-party/maplibre-native'
maplibre_flutter_root = maplibre_root / 'platform/flutter'

folders_to_clean = []
for item in maplibre_root.iterdir():
    if item.is_dir() and item.name.startswith('build-'):
        folders_to_clean.append(item)

if maplibre_flutter_root.exists():
  folders_to_clean.append(maplibre_flutter_root)

if not folders_to_clean:
    print('No build folders found to clean.')
    exit(0)

print('Cleaning: ')
for folder in folders_to_clean:
    print(f' - {folder}')

print()
print('Are you sure you want to proceed? (y/N): ', end='')
response = input().strip().lower()
if response != 'y':
    print('Aborting.')
    exit(0)

for folder in tqdm(folders_to_clean, desc="Cleaning build folders"):
    if folder.exists() and folder.is_dir():
        subprocess.run(shlex.split(f'rm -rf "{folder}"'))
