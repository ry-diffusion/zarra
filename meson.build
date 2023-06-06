project('zarra', 'c',
  version : '23.06-unstable',
  default_options : ['warning_level=3'])

conf_data = configuration_data()
conf_data.set('version','23.06-unstable')

configure_file(input : 'include/zarra_static.h.in',
               output : 'zarra_static.h',
               configuration : conf_data)

executable('zarra', [ 'src/main.c', 'src/cli.c', 'src/utils.c', 'src/task.c', 'src/ffmpeg_parser.c' ] ,
  include_directories : 'include',
  install : true)


