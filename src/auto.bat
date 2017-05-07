@echo off
pushd ..
make
popd
exit %errorlevel%
