import os
import string
import glob

from fabric.api import *

nginx_pkg = 'http://nginx.org/download/nginx-1.4.0.tar.gz'
openresty_pkg = 'http://agentzh.org/misc/nginx/ngx_openresty-1.2.4.14.tar.gz'

"""
#Command('speed_analysis.tex','code/speed.py','python code/speed.py')

#myfile = env.Command('myfile.out', [], 'echo Hello world > $TARGETS')
#env.AlwaysBuild(myfile)
#./configure --with-ld-opt="-lstdc++" --add-module=../src/demoj

./configure --with-luajit --prefix=%s >/dev/null' % installdir, 
		 'make >/dev/null', 'make install >/dev/null'
"""


def deps():
	local('cd deps;wget %s' % nginx_pkg)
	local('cd deps;wget %s' % openresty_pkg)

def install(dest, resty=True, module=True):

	luaconf = '--with-luajit' if resty is True else ''
	moduleconf = ''
	if module:
		moduleconf = '--with-ld-opt="-lstdc++" '	
		modules = glob.glob('src/cpp/*')
		for dir in modules:
			moduleconf += ' --add-module=%s' % os.path.abspath(dir)

	config_command = './configure %s %s --prefix=%s' % (luaconf, moduleconf, dest)

	pkg = 'ngx_openresty' if resty is True else 'nginx'
	local('cd deps; tar zxvf %s*.tar.gz' % pkg)
	local('cd deps/%s*; %s; make; make install' % (pkg, config_command))

        local('cp -rf conf %s' % dest)
        put('cp -rf src/lua %s' % dest)
	local('ln -s %s/lua %s/nginx/lua' % (dest, dest))

