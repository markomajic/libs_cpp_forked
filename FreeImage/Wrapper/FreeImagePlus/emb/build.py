import sys, os, subprocess, getopt, shutil, glob, rsvars, rsbuild

from rsbuild import TargetProps
from rsbuild import RSBuild
from rsbuild import CustomJob
from rsbuild import CMakeJob

class TestRunJob(CustomJob):
	def __init__(self, target):
		super().__init__(target)

	def get_job_caption(self):
		return 'test runs'

	def clean(self):
		return True # nothing to clean up for test runs (old results are automatically cleaned up by TestRunner class)

	def build(self):
		print('Running tests...')
		exe_dir = self.target.build_dir
		try:
			if os.path.isdir(exe_dir):
				os.chdir(exe_dir)
			else:
				print('Unable to locate bin directory! Are we in a right place?')
				return False

			test_exe_files = glob.glob("fip*.exe")
			if len(test_exe_files) == 0:
				print('No test executables found here! Are we  in a right place?')
				return False

			# run all the tests (regardless if some fail) along the way. the final res_code will be 0
			# only if all the tests executed successfully
			res_code = 0
			for test_exe in test_exe_files:
				print(test_exe)
				res_code = max(subprocess.run(test_exe, shell=True).returncode, res_code)
			return res_code == 0

		except OSError as os_err:
			print(os_err)
			return False

class FIPBuild(RSBuild):
	def __init__(self, argv):
		rsbuild.has_init = True
		rsbuild.build_root = 'builds\\emb'
		rsbuild.valid_configs.remove(rsvars.cnfDbg)

		rsbuild.target_props.append(TargetProps(name='dll', src_dir='src'))
		rsbuild.target_props.append(TargetProps(name='test', src_dir='test'))
		rsbuild.target_props.append(TargetProps(name='run', src_dir='src', cmake=False, build_alias='test'))

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
		if target.name == 'run':
			return TestRunJob(target)

		if target.cmake:
			if target.name == 'dll':
				target.params.append('CMAKE_ARCHIVE_OUTPUT_DIRECTORY=' + os.environ['LIBOutput'])
				target.params.append('CMAKE_RUNTIME_OUTPUT_DIRECTORY=' + os.environ['DLLOutput'])

		return super().create_job(target)

if __name__ == '__main__':
	build_job = FIPBuild(sys.argv)
	sys.exit(0 if build_job.run() else 1)
