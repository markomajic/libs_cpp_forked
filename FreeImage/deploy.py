import sys, os, subprocess, getopt, shutil, glob, rsvars, rsbuild

from rsbuild import TargetProps
from rsbuild import RSBuild
from rsbuild import CustomJob
from rsbuild import CMakeJob

class DeployJob(CustomJob):
	def __init__(self, target):
		self.fi_hdr = 'FreeImage.h'
		self.fip_hdr = 'FreeImagePlus.h'
		self.dll_name = 'FreeImage.dll'
		super().__init__(target)

	def get_job_caption(self):
		return 'deploy FreeImage'

	def clean(self):
		return True # nothing to clean up for test runs (old results are automatically cleaned up by TestRunner class)

	def build(self):
		src_dir = os.path.join(self.target.src_dir, 'x64' if self.target.platform=='Win64' else 'x32')
		print(src_dir)
		return True

class FIDeploy(RSBuild):
	def __init__(self, argv):
		rsbuild.has_init = False
		rsbuild.build_root = '.' # not really used here
		rsbuild.valid_configs.remove(rsvars.cnfDbg)

		rsbuild.target_props.append(TargetProps(name='deploy', src_dir='dist', cmake=False))
		# explicitly setting build_dir will prevent auto-expansion of Platform and Config parameters in prepare()
		# but will still expand the path relative to home_dir...
		rsbuild.target_props[0].build_dir = 'Wrapper/FreeImagePlus' 
		super().__init__(argv)

	def create_job(self, target):
		if target.name == 'deploy':
			return DeployJob(target)

if __name__ == '__main__':
	sys.argv.append('--platform=Win32,Win64')
	build_job = FIDeploy(sys.argv)
	sys.exit(0 if build_job.run() else 1)
