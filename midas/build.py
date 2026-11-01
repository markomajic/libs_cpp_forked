import sys, os, subprocess, getopt, shutil, glob, rsvars, rsbuild

from rsbuild import TargetProps
from rsbuild import RSBuild
from rsbuild import CustomJob
from rsbuild import CMakeJob

class MidasBuild(RSBuild):
	def __init__(self, argv):
		rsbuild.has_init = True
		rsbuild.build_root = 'builds'
		rsbuild.valid_configs.remove(rsvars.cnfDbg)

		rsbuild.target_props.append(TargetProps(name='lib', src_dir='.'))

		super().__init__(argv)

	def custom_init(self):
		try:
			if os.path.isdir(rsbuild.build_root):
				shutil.rmtree(rsbuild.build_root)
				print('Build environment initialized (all build destination directories have been deleted)...')
			else:
				print('Nothing to do (build destination directories not present)...')
			return True
		except OSError as os_err:
			print(os_err)
			return False

	def create_job(self, target):
		if target.cmake:
			if target.name == 'lib':
				target.params.append('CMAKE_ARCHIVE_OUTPUT_DIRECTORY=' + os.environ['LIBOutput'])
				# target.params.append('CMAKE_RUNTIME_OUTPUT_DIRECTORY=' + os.environ['BPLOutput'])

		return super().create_job(target)

if __name__ == '__main__':
	build_job = MidasBuild(sys.argv)
	sys.exit(0 if build_job.run() else 1)
