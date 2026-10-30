"""Run DSP suites explicitly in each Mac architecture, including Rosetta."""
import argparse
import pathlib
import subprocess
import sys
p=argparse.ArgumentParser()
p.add_argument('build',type=pathlib.Path)
p.add_argument('--arch',choices=['arm64','x86_64','both'],default='both')
a=p.parse_args()
tests=['dsp_tests','preset_tests','regression_tests','voicing_tests','wet_diffusion_tests','character_duck_tests','ping_pong_tests','texture_tests','tone_filter_tests']
for arch in (['arm64','x86_64'] if a.arch=='both' else [a.arch]):
 for name in tests:
  print(f'RUN {arch}: {name}',flush=True)
  r=subprocess.run(['/usr/bin/arch',f'-{arch}',str(a.build.resolve()/name)],timeout=600)
  if r.returncode: sys.exit(r.returncode)
print('PASS: all requested architectures',flush=True)
