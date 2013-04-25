import os
import string

from fabric.api import *
from fabset.openresty import *


name = 'blobserve'
version = '0.1.0' 
deploydir = '/opt/space/blobserve' 

nginx_pkg = 'http://nginx.org/download/nginx-1.4.0.tar.gz'
openresty_pkg = 'http://agentzh.org/misc/nginx/ngx_openresty-1.2.4.14.tar.gz'


def deps():
	
	run()

def build():

def install():
	run('mkdir -p %s' % deploydir)
	run('rm -rf %s' % installdir)
	run('mkdir -p %s' % installdir)

	install_openresty(openrestydir, openresty_pkg)

        put('conf', installdir)
        put('lua', installdir)
	run('rm -f %s/nginx/conf/nginx.conf' % openrestydir)
	run('ln -s %s/conf/nginx.conf %s/nginx/conf/nginx.conf' % (installdir, openrestydir))
	run('ln -s %s/lua %s/nginx/lua' % (installdir, openrestydir))
	

