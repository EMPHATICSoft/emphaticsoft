eval 'vecho_() { test -n "${vv-}" && echo ">> $*"; return 0; }' && \
eval 'setenv() { eval "$1=\"\${2-}\"; export $1"; }' && \
eval 'unsetenv_() { unset "$@"; }' && \
eval 'source() { . "$@"; }' && \
eval 'tnotnull() { eval "test -n \"\${$1-}\""; }' && \
eval 'nullout() { "$@" >/dev/null 2>&1; }'

myDir=$(dirname ${BASH_SOURCE})
cd ${myDir}/..

setenv CETPKG_SOURCE $(pwd)

cd -

setenv CETPKG_BUILD $(pwd)
