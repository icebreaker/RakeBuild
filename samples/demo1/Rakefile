#
#	RakeBuild 'Rakefile'
#
#	Copyright (c) 2010, Mihail Szabolcs
#	All rights reserved.
#
#	Redistribution and use in source and binary forms, with or
#	without modification, are permitted provided that the following
#	conditions are met:
#
#	* 	Redistributions of source code must retain the above copyright
#		notice, this list of conditions and the following disclaimer.
#
#	* 	Redistributions in binary form must reproduce the above copyright
#		notice, this list of conditions and the following disclaimer in
#		the documentation and/or other materials provided with the
#		distribution.
#
#	* 	Neither the name of the RakeBuild nor the names of its contributors
#		may be used to endorse or promote products derived from this
#		software without specific prior written permission.
#
#	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
#	OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
#	THE POSSIBILITY OF SUCH DAMAGE.
#
require 'yaml'
require 'rake/clean'

# Terminal Colors
# TODO: make these configurable from Global Configuration
COLOR_PURPLE	= '\x1b[35m'
COLOR_BLUE 		= '\x1b[34m'
COLOR_GREEN 	= '\x1b[32m'
COLOR_YELLOW 	= '\x1b[33m'
COLOR_RED		= '\x1b[31m'
COLOR_END		= '\x1b[0m'

# Current Working Directory
CWD = File.dirname(__FILE__)

# Default Global Configuration Filename
CONFIG_FILE = "#{CWD}/#{File.basename(CWD)}.yaml"

# Default Application Configuration
DEFAULT_CONFIG_APP = {
	'dir'=>[{'src'=>'build'}],
	'type'=>'app',
	'name'=>'',
	'cc'=>'g++',
	'cc-flags'=>'-Wall -Wextra -W -O2 -c -o #{_name} #{_src}',
	'cc-flags-extra'=>'',
	'linker'=>'g++',
	'linker-flags'=>'-o #{name} #{obj}',
	'linker-flags-extra'=>'',
	'src'=>['cpp','c','cxx','cc'],
	'obj'=>'o',
	'prefix'=>'',
	'suffix'=>'',
	'deps'=> [],
	'autodeps'=> true
}
# Default Static Library Configuration (merged with the Application)
DEFAULT_CONFIG_STATIC = {
	'linker'=>'ar',
	'linker-flags'=>'rcs #{name} #{obj}',
	'prefix'=>'lib',
	'suffix'=>'a'
}
# Default Shared Object Configuration (merged with the Application)
DEFAULT_CONFIG_SHARED = {
	'linker-flags' => '-shared -o #{name} #{obj}',
	'prefix'=>'lib',
	'suffix'=>'so'
}

#
# Method: log
#	Output to stdout helper.
#
# Paramaters:
#	message - text / message to be shown
#
def log(message)
	puts(" => #{message.gsub(CWD+'/','')} ...")
end
 
# Load / Override Global Configuration
if File.exists?(CONFIG_FILE)
	# TODO: handle possible exceptions gracefully
	config = YAML.load_file(CONFIG_FILE) || {}
	
	CONFIG_APP 		= DEFAULT_CONFIG_APP.merge(config)
	CONFIG_STATIC 	= CONFIG_APP.merge(DEFAULT_CONFIG_STATIC.merge(config))
	CONFIG_SHARED 	= CONFIG_APP.merge(DEFAULT_CONFIG_SHARED.merge(config))
	 
	log("RakeBuild configuration loaded")
else
	CONFIG_APP 		= DEFAULT_CONFIG_APP
	CONFIG_STATIC	= CONFIG_APP.merge(DEFAULT_CONFIG_STATIC)
	CONFIG_SHARED	= CONFIG_APP.merge(DEFAULT_CONFIG_SHARED)
	
	log("RakeBuild using default configuration")
end

#
# Method: log_color
#	Colored Output to stdout helper.
#
# Paramaters:
#	action_color 	- color of the action
#	action			- text to be shown as action
#	message_color	- color of the message
#	message 		- text to be shown as message
#
def log_color(action_color, action, message_color='', message='')
	# No Colored Output?!
	if CONFIG_APP['nocolors'] == true
		log("#{action} #{message}")
		return
	end

	# Colored Output
	log(eval('"'+action_color + action + message_color + ' ' + message + COLOR_END+'"'))
end

#
# Function: LoadProject
#	Load a RakeBuild Project in YAML format.
#
# Paramaters:
#	proj 	- absolute project file path
#
# Returns:
#	- nil if project doesn't exists
#	- empty hash if the project file is empty
#	- or hash with YAML key value pairs
#
def LoadProject(proj)
	unless File.exists?(proj)
		return nil
	end

	return YAML.load_file(proj) || {};
end
#
# Function: IsDebugProject
#	Tells whatever a RakeBuild Project is debug or not.
#
# Paramaters:
#	proj 	- absolute project file path
#
# Returns:
#	- nil or true (pos)
#
def IsDebugProject(proj)
	return proj =~ /-debug\.yaml$/
end
#
# Function: ProcessProjectDependencies
#	Process a RakeBuild Project's dependencies returning linker flags
#
# Paramaters:
#	source 	- absolute top level source directory
#	build	- absolute top level build directory
#	deps	- dependency name
# 	debug 	- debug mode on / off
#
# Returns:
#	- nil or linker flags
#
def ProcessProjectDependencies(source, build, dep, debug)

	# Release Project File
	_release = "#{source}/#{dep}/#{dep}.yaml";
	# Debug Project File
	_debug = "#{source}/#{dep}/#{dep}-debug.yaml"
	
	# 
	is_debug = debug
	
	if is_debug
		_config = LoadProject(_debug)
		if _config == nil
			_config = LoadProject(_release)
			is_debug = false
		end
	else
		_config = LoadProject(_release)
		if _config == nil
			_config = LoadProject(_debug)
			is_debug = true
		end
	end

	unless _config.nil?
		_type = _config['type']	
		unless _type.nil? or _type == '' or _type == 'app'
			if is_debug
				return "-L#{build}/debug/#{dep} -l#{dep}"
			else
				return "-L#{build}/release/#{dep} -l#{dep}"
			end
		end
	end

	return nil
end
#
# Function: ProcessProject
#	Process a RakeBuild Project in YAML format.
#
# Paramaters:
#	source 	- absolute top level source directory
#	build	- absolute top level build directory
#	proj	- absolute project file path
# 
# Returns:
# 	task name
#
def ProcessProject(source, build, proj)

	# Load RakeBuild Project
	_config = LoadProject(proj);

	# Failed ...
	if _config == nil
		return nil
	end

	# Merge the initial configuration
	config = CONFIG_APP.merge( _config )

	log_color(COLOR_BLUE,"Loading Project '#{proj}'")

	# Debug or Release
	debug = IsDebugProject(proj)
	# Project Source Directory
	source_dir 	= File.dirname(proj)
	# Project Build Directory
	if debug
		build_dir 	= source_dir.gsub(source, "#{build}/debug")
	else
		build_dir 	= source_dir.gsub(source, "#{build}/release")
	end

	# Link Text
	link_text = "Linking Custom Target >> '#{config['type']}'"

	# Target Type
	case config['type']
	
		when 'static' # Static Library
			link_text = 'Linking Static Library'
			config = CONFIG_STATIC.merge( _config ) # merge static library specifics
		when 'shared' # Shared Object (Library)
			link_text = 'Linking Shared Object'
			config = CONFIG_SHARED.merge( _config ) # merge shared library specifics
		when 'app' # Executable Application
			link_text = 'Linking Executable' # we merged earlier ...
	end

	# Set a valid Target Name Automatically
	if config['name'].nil? or config['name'] == ''
		config['name'] = File.basename(source_dir)
	end

	# Grab 'Target' Name
	name = config['name']
	# Prepend Prefix
	unless config['prefix'].nil? or config['prefix'] == ''
		name = config['prefix'] + name
	end
	# Append Extension
	unless config['suffix'].nil? or config['suffix'] == ''
		name = "#{name}.#{config['suffix']}"
	end

	# Absolute Target Name
	name = File.join(build_dir, name)

	# Process Compile flags
	ccflags = config['cc-flags']
	# Process Extra Compile flags
	unless config['cc-flags-extra'].nil? or config['cc-flags-extra'] == ''
		ccflags = config['cc-flags-extra'] + ' ' + ccflags
	end

	# Compiler Command
	cc = config['cc'] + ' ' + ccflags
	
	sources = []
	config['src'].each do |ext|
		sources.push("#{source_dir}/*.#{ext}")
	end

	# Get Source File list
	src = FileList[sources]

	task "#{name}-compile" do
		src.each do |f|
			# Used for templating purposes in the cc-flags and cc-flags-extra
			_name = f.ext(config['obj']).gsub(source_dir, build_dir)
			_src = f

			# Perform Object Compiling
			log_color(COLOR_PURPLE,'Compiling',COLOR_BLUE,"'"+_src+"'")
			system(eval('"'+cc+'"'))
		end
	end

	# Create Build Directory
	directory "#{build_dir}"
	file "#{name}-create-dir" => ["#{build_dir}"] do |t|
	end

	# Add Create Directory Task
	task "#{name}-compile" => ["#{name}-create-dir"]
	
	# 
	obj = src.ext(config['obj']).gsub(source_dir, build_dir)

	# Clean Temporary Object Files
	CLEAN.include(File.join(build_dir, "*.#{config['obj']}"))
	# Clean Target File(s)
	CLOBBER.include(name)

	# Task
	task "#{config['name']}" do |t|
		log_color(COLOR_YELLOW,"#{t.name} Finished")
	end

	# Process Linker flags
	linkerflags = config['linker-flags']
	# Process Extra Linker flags
	unless config['linker-flags-extra'].nil? or config['linker-flags-extra'] == ''
		linkerflags = config['linker-flags-extra'] + ' ' + linkerflags
	end

	# Loop through dependecy list ...
	config['deps'].each do |dep|
		# Add Dependency
		task "#{name}-compile" => [dep]
		
		# Link Dependencies Automatically?
		if config['autodeps'] == true
			_linkerflags = ProcessProjectDependencies(source, build, dep, debug)
			unless _linkerflags.nil?
				linkerflags = linkerflags + " " + _linkerflags;
			end
		end
	end

	# Link Command
	link = config['linker'] + ' ' + linkerflags

	task "#{name}-link" do
		log_color(COLOR_RED, link_text, COLOR_GREEN, "'#{name}'");
		system(eval('"'+link+'"'))
	end

	# Linking depends on Compiling
	task "#{name}-link" => ["#{name}-compile"]

	# Add Linker Task
	task "#{config['name']}" => ["#{name}-link"]

	# Return Task Name
	return "#{config['name']}"
end

def ScanProjects(source, build, debug=false)
	# Tasks
	tasks = []
	# Release Project File
	proj_release = '#{proj_dir}/#{File.basename(proj_dir)}.yaml'
	# Debug Project File
	proj_debug = '#{proj_dir}/#{File.basename(proj_dir)}-debug.yaml'
		
	# File List Pattern 
	FileList["#{source}/*"].each do |proj_dir|
		# Is this a directory?
		if FileTest.directory?(proj_dir)
			# Process Project File
			if debug # Debug
			
				task = ProcessProject(source, build, eval('"'+proj_debug+'"'))
				if task == nil
					release_found = true
					task = ProcessProject(source, build, eval('"'+proj_release+'"'))
				end
								
			else # Release
				task = ProcessProject(source, build, eval('"'+proj_release+'"'))
				if task == nil
					debug_found = true
					task = ProcessProject(source, build, eval('"'+proj_debug+'"'))
				end
			end
			
			unless task == nil
				tasks.push( task )

				if debug_found == true
					log("=> No Release Project found using Debug Project")
				end
				if release_found == true
					log("=> No Debug Project found using Release Project")
				end
			end
		end
	end
	return tasks
end

# Task Names in Release
release_tasks = []
# Task Names in Debug
debug_tasks = []

# Scan for Configured Project Directories / Modules
CONFIG_APP['dir'].each do |dirs|
	dirs.each do |src, build|
		# Absolute source directory
		src_dir = "#{CWD}/#{src}"
		# Absolute build directory
		build_dir = "#{CWD}/#{build}"

		log('RakeBuild Loading Release Projects')
		# Build Release Namespace
		namespace 'Release' do
			release_tasks = ScanProjects(src_dir, build_dir)			
		end
		log('RakeBuild Loading Debug Projects')
		# Build Debug Namespace
		namespace 'Debug' do
			debug_tasks = ScanProjects(src_dir, build_dir, true)
		end
		log('RakeBuild Projects Loaded')
	end
end

# Add Release tasks
release_tasks.each do |t|
	task :release => ["Release:#{t}"]
end

# Add Debug Tasks
debug_tasks.each do |t|
	task :debug => ["Debug:#{t}"]
end

desc 'Build debug'
task :debug do
	# Finished
	log('RakeBuild Debug Finished')
end

desc 'Build release'
task :release do
	# Finished
	log('RakeBuild Release Finished')
end

# Default Executes Build Release
task :default => ['release']

# Default Task
desc 'Build release'
task :default do
	log('RakeBuild Finished')
end
