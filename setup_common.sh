########################################################################

####################################
# Personality check: are we setup_deps or setup_for_development?
#
# If we're sourcing setup.in directly (e.g. for cetmodules itself) then
# we select "setup_for_development" behavior, otherwise _cetpkg_nobuild will be
# set appropriately by CMake's CONFIGURE() command.

test -z "$_cetpkg_nobuild" -o "$_cetpkg_nobuild" = "--nobuild" || \
  _cetpkg_nobuild="" || true
####################################

####################################
# Initial info.

printf "\n";
test -z "$_cetpkg_nobuild" && \
  echo "The working build directory is $CETPKG_BUILD" || true
cat <<EOF
The source code directory is $CETPKG_SOURCE

---------------------- check this block for errors ---------------------
EOF

_cetpkg_status=""
####################################

####################################
# Attempt to find a suitable cetmodules to set up.

_cetpkg_cetmsetup=`sed -nE 's&^[[:space:]]*(cetmodules)[[:space:]]+([^[:space:]]+).*\bonly_for_build\b.*$&\1 \2&p' "$CETPKG_SOURCE/ups/product_deps"`
test -z "_cetpkg_cetmsetup" -a ! -x "$CETPKG_SOURCE/libexec/set_dev_products" && \
  _cetpkg_status=1 && \
  echo "ERROR: unable to find suitable cetmodules >= 2.00.00" || true
test -n "$_cetpkg_status" -o -z "$_cetpkg_cetmsetup" || setup -B $_cetpkg_cetmsetup
test $? != 0 && _cetpkg_status=1 && \
  echo "ERROR: setup of $_cetpkg_cetmsetup has failed" || true
test -z "$_cetpkg_status$_cetpkg_cetmsetup" && setenv PATH `dropit -sfe "$CETPKG_SOURCE/bin"` && _cetpkg_cettop="$CETPKG_SOURCE" || true
test -z "$_cetpkg_status$_cetpkg_cettop" -a -n "$CETMODULES_DIR" && _cetpkg_cettop="$CETMODULES_DIR"
####################################

####################################
# Generate the setup script to set up dependencies.

test -n "$_cetpkg_status" || \
  _cetpkg_setup="`${_cetpkg_cettop}/libexec/set_dev_products $_cetpkg_nobuild -p`"
test $? != 0 -o -z "$_cetpkg_setup" && _cetpkg_status=1 && \
  echo "ERROR: generation of setup script has failed" || true
####################################

####################################
# Source the setup script to set up dependencies.

test -n "$_cetpkg_status" || source "$_cetpkg_setup"
test $? != 0 && _cetpkg_status=1 && \
  echo "ERROR: setup of required products has failed" || true
_cetpkg_setup_errlog="$CETPKG_BUILD/cetpkg_setup.err"
test -s "$_cetpkg_setup_errlog" && cat "$_cetpkg_setup_errlog" || \
  rm -f "$_cetpkg_setup_errlog"
####################################

cat <<EOF
------------------------------------------------------------------------

EOF

# Final sanity check and report.
test -n "$_cetpkg_status" -o -n "$_cetpkg_nobuild" || \
  "$_cetpkg_cettop/libexec/verify_build_environment" "$CETPKG_BUILD"
test $? != 0 && _cetpkg_status=1 && \
  echo "ERROR: unable to verify build environment" || true

####################################
# Clean up before exiting.

# Clean up C-flavor aliases and prepare to return^H^H^H^H^H^Hexit.
test "$_cetpkg_shell_flavor" = csh && \
  unalias vecho_ unsetenv_ tnotnull nullout __TOP_EOF__ return && \
  set return=exit || return=return

# Finish cleanup and exit. Note the use of eval so we can unset
# everything including $return and $_cetpkg_status.
eval "unset vecho_ setenv unsetenv_ source tnotnull nullout "\
"_cetpkg_nobuild _cetpkg_shell_var_set "\
"_cetpkg_fw_db _cetpkg_myself _cetpkg_mydir _cetpkg_cetmsetup "\
"_cetpkg_cettop _cetpkg_setup _cetpkg_setup_errlog "\
"_cetpkg_shell_flavor return _cetpkg_status; \
$return $_cetpkg_status"
####################################

########################################################################
\__TOP_EOF__
