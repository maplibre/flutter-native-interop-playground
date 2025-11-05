import subprocess
import shlex
import pathlib

plugin_root = pathlib.Path(__file__).parent.parent

subprocess.run(
  shlex.split('fvm dart run ffigen --config ffigen.yaml'),
  check=True,
  cwd=plugin_root
)