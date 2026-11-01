import sys, os, subprocess, getopt, shutil, glob, rsvars, rsbuild

from rsbuild import TargetProps
from rsbuild import RSBuild
from rsbuild import CMakeJob

class UDT4Build(RSBuild):
	def __init__(self, argv):
		rsbuild.has_init = True
		rsbuild.build_root = 'builds/emb'
		rsbuild.valid_configs.remove(rsvars.cnfDbg)

		rsbuild.target_props.append(TargetProps(name='dll', src_dir='src'))
		rsbuild.target_props.append(TargetProps(name='app', src_dir='app'))

		super().__init__(argv)

	def custom_init(self):
		for target in rsbuild.target_props:
			if target.cmake:
				init_cmake_path = os.path.join(self.script_dir, target.name + '.cmake')
				dest_cmake_path = os.path.join(target.src_dir, rsbuild.cmlists_file)
				if not os.path.isfile(init_cmake_path):
					print('Initialization files missing: Template cmake file (', init_cmake_path, ') not found!', sep='')
					return False
				
				shutil.copyfile(init_cmake_path, dest_cmake_path)
		return True

	def create_job(self, target):
		if target.name == 'app':
			# if target is 'app' we need to pass the location of source directory
			# for the include_directories() statement...
			lib_target = rsbuild.find_target_props('dll')
			target.params.append('lib_src_dir='+lib_target.src_dir)
		return super().create_job(target)

if __name__ == '__main__':
	build_job = UDT4Build(sys.argv)
	sys.exit(0 if build_job.run() else 1)
