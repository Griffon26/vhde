# Installing dependencies

Some of VHDE's dependencies are not available in all linux distributions.
An example of such a distribution is Ubuntu 14.04 (Trusty), which is used by
Travis.

To easily install these dependencies for Travis some scripts were created,
but these scripts can be just as useful if your distribution is missing
the required dependencies.

## env.sh

To make sure that dependencies can be found after they have been installed,
you'll have to modify your `PKG_CONFIG_PATH` and `LD_LIBRARY_PATH` environment
variables. The following command will do that:

    source env.sh

## install-from-github.sh

The [`install-from-github.sh`](install-from-github.sh) script can be used to
install a Github project that is based on autotools into your home dir under
`~/.local`. It will download, extract and build the project in a `tmp/`
directory below the directory where the script itself is located.

```
Usage: install-from-github.sh <project> <package> [version] [-p <patch>]

  project    : github project name
  package    : github repository name
  version    : an optional version of the package to use (must be a tag or branch). Defaults to "master"
  -p <patch> : an optional patch that will be applied to the source code before the build
```

Take a look at the [`.travis.yml`](/.travis.yml) file in the root of this
repository to see some example invocations of this script. In addition to those
invocations the script has also been used to install `mm-common` and may work
for other dependencies that your distribution does not provide.


