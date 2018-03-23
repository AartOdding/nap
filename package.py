#!/usr/bin/env python
import argparse
import datetime
import json
import os
from multiprocessing import cpu_count
import shutil
import subprocess
import sys
from sys import platform

WORKING_DIR = '.'
BUILD_DIR = 'packagingBuild'
LIB_DIR = 'packagingLib'
BIN_DIR = 'packagingBin'
PACKAGING_DIR = 'packagingStaging'
ARCHIVING_DIR = 'archiving'
BUILDINFO_FILE = 'dist/cmake/buildInfo.json'
BUILD_TYPES = ('Release', 'Debug')

def call(cwd, cmd, capture_output=False, exception_on_nonzero=True):
    """Execute command in provided working directory"""

    # print('dir: %s' % cwd)
    # print('cmd: %s' % cmd)
    if capture_output:
        proc = subprocess.Popen(cmd, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    else:
        proc = subprocess.Popen(cmd, cwd=cwd)
    (out, err) = proc.communicate()
    if exception_on_nonzero and proc.returncode != 0:
        print("Bailing for non zero returncode")
        raise Exception(proc.returncode)
    return (out, err)

def package(zip_release, include_docs, include_apps, clean):
    """Package a NAP platform release - main entry point"""

    print("Packaging...")
    os.chdir(os.path.dirname(os.path.realpath(__file__)))

    # Remove old packaging path if it exists
    if os.path.exists(PACKAGING_DIR):
        shutil.rmtree(PACKAGING_DIR, True)
    os.makedirs(PACKAGING_DIR)

    # Clean build if requested
    if clean:
        print("Cleaning...")
        if platform.startswith('linux'):    
            for build_type in BUILD_TYPES:
                if os.path.exists(BUILD_DIR + build_type):
                    print("Clean removing %s" % os.path.abspath(BUILD_DIR + build_type))
                    shutil.rmtree(BUILD_DIR + build_type, True)
        else:
            if os.path.exists(BUILD_DIR):
                print("Clean removing %s" % os.path.abspath(BUILD_DIR))
                shutil.rmtree(BUILD_DIR, True)

        if os.path.exists(LIB_DIR):
            print("Clean removing %s" % os.path.abspath(LIB_DIR))
            shutil.rmtree(LIB_DIR, True)
        if os.path.exists(BIN_DIR):
            print("Clean removing %s" % os.path.abspath(BIN_DIR))
            shutil.rmtree(BIN_DIR, True)                

    # Add timestamp and git revision for build info
    timestamp = datetime.datetime.now().strftime('%Y.%m.%dT%H.%M')
    (git_revision, _) = call(WORKING_DIR, ['git', 'rev-parse', 'HEAD'], True)
    git_revision = git_revision.decode('ascii', 'ignore').strip()

    # Do the packaging
    if platform.startswith('linux'):    
        package_for_linux(timestamp, git_revision, include_apps, include_docs, zip_release)
    elif platform == 'darwin':
        package_for_macos(timestamp, git_revision, include_apps, include_docs, zip_release)
    else:
        package_for_win64(timestamp, git_revision, include_apps, include_docs, zip_release)


def package_for_linux(timestamp, git_revision, include_apps, include_docs, zip_release):
    """Package NAP platform release for Linux"""

    for build_type in BUILD_TYPES:
        build_dir_for_type = BUILD_DIR + build_type
        call(WORKING_DIR, ['cmake', 
                           '-H.', 
                           '-B%s' % build_dir_for_type, 
                           '-DNAP_PACKAGED_BUILD=1',
                           '-DCMAKE_BUILD_TYPE=%s' % build_type,
                           '-DINCLUDE_DOCS=%s' % int(include_docs),
                           '-DPACKAGE_NAIVI_APPS=%s' % int(include_apps),
                           '-DBUILD_TIMESTAMP=%s' % timestamp,
                           '-DBUILD_GIT_REVISION=%s' % git_revision
                           ])

        d = '%s/%s' % (WORKING_DIR, build_dir_for_type)
        call(d, ['make', 'all', 'install', '-j%s' % cpu_count()])

    # Create archive
    if zip_release:
        archive_to_linux_tar_xz(timestamp)
    else:
        archive_to_timestamped_dir('Linux', timestamp)

def package_for_macos(timestamp, git_revision, include_apps, include_docs, zip_release):
    """Package NAP platform release for macOS"""

    # Generate project
    call(WORKING_DIR, ['cmake', 
                       '-H.', 
                       '-B%s' % BUILD_DIR, 
                       '-G', 'Xcode',
                       '-DNAP_PACKAGED_BUILD=1',                 
                       '-DINCLUDE_DOCS=%s' % int(include_docs),
                       '-DPACKAGE_NAIVI_APPS=%s' % int(include_apps),
                       '-DBUILD_TIMESTAMP=%s' % timestamp,
                       '-DBUILD_GIT_REVISION=%s' % git_revision
                       ])

    # Build & install to packaging dir
    d = '%s/%s' % (WORKING_DIR, BUILD_DIR)
    for build_type in BUILD_TYPES:
        call(d, ['xcodebuild', '-configuration', build_type, '-target', 'install', '-jobs', str(cpu_count())])

    # Remove unwanted files (eg. .DS_Store)
    call(PACKAGING_DIR, ['find', '.', '-name', '.DS_Store', '-type', 'f', '-delete'])

    # Create archive
    if zip_release:
        archive_to_macos_zip(timestamp)
    else:
        archive_to_timestamped_dir('macOS', timestamp)

def package_for_win64(timestamp, git_revision, include_apps, include_docs, zip_release):
    """Package NAP platform release for Windows"""

    # Create build dir if it doesn't exist
    if not os.path.exists(BUILD_DIR):
        os.makedirs(BUILD_DIR)

    # Generate project
    call(WORKING_DIR, ['cmake', 
                       '-H.', 
                       '-B%s' % BUILD_DIR, 
                       '-G', 'Visual Studio 14 2015 Win64',
                       '-DNAP_PACKAGED_BUILD=1',
                       '-DPYBIND11_PYTHON_VERSION=3.5',
                       '-DINCLUDE_DOCS=%s' % int(include_docs),
                       '-DPACKAGE_NAIVI_APPS=%s' % int(include_apps),
                       '-DBUILD_TIMESTAMP=%s' % timestamp,
                       '-DBUILD_GIT_REVISION=%s' % git_revision
                       ])

    # Build & install to packaging dir
    for build_type in BUILD_TYPES:
        call(WORKING_DIR, ['cmake', '--build', BUILD_DIR, '--target', 'install', '--config', build_type])

    # Create archive
    if zip_release:
        archive_to_win64_zip(timestamp)
    else:
        archive_to_timestamped_dir('Win64', timestamp)

def archive_to_linux_tar_xz(timestamp):
    """Create build archive to xz tarball on Linux"""

    package_filename = build_package_basename('Linux', timestamp)
    shutil.move(PACKAGING_DIR, package_filename)

    package_filename_with_ext = '%s.%s' % (package_filename, 'tar.xz')
    print("Archiving to %s ..." % os.path.abspath(package_filename_with_ext))
    call(WORKING_DIR, ['tar', '-cJvf', package_filename_with_ext, package_filename])

    # Cleanup
    shutil.move(package_filename, PACKAGING_DIR)
    print("Packaged to %s" % os.path.abspath(package_filename_with_ext))

def archive_to_macos_zip(timestamp):
    """Create build archive to zip on macOS"""

    package_filename = build_package_basename('macOS', timestamp)
    shutil.move(PACKAGING_DIR, package_filename)

    # Archive
    package_filename_with_ext = '%s.%s' % (package_filename, 'zip')
    print("Archiving to %s ..." % os.path.abspath(package_filename_with_ext))
    call(WORKING_DIR, ['zip', '-yr', package_filename_with_ext, package_filename])

    # Cleanup
    shutil.move(package_filename, PACKAGING_DIR)
    print("Packaged to %s" % os.path.abspath(package_filename_with_ext))  

def archive_to_win64_zip(timestamp):
    """Create build archive to zip on Win64"""

    package_filename = build_package_basename('Win64', timestamp)
    package_filename_with_ext = '%s.%s' % (package_filename, 'zip')

    # Rename our packaging dir to match the release
    shutil.move(PACKAGING_DIR, package_filename)

    # Create our archive dir, used to create a copy level folder within the archive
    if os.path.exists(ARCHIVING_DIR):
        shutil.rmtree(ARCHIVING_DIR, True)
    os.makedirs(ARCHIVING_DIR)
    archive_path = os.path.join(ARCHIVING_DIR, package_filename)
    shutil.move(package_filename, archive_path)

    # Create archive
    print("Archiving to %s ..." % os.path.abspath(package_filename_with_ext))
    shutil.make_archive(package_filename, 'zip', ARCHIVING_DIR)

    # Cleanup
    shutil.move(archive_path, PACKAGING_DIR)
    shutil.rmtree(ARCHIVING_DIR)

    print("Packaged to %s" % os.path.abspath(package_filename_with_ext))  

def archive_to_timestamped_dir(platform, timestamp):
    """Copy our packaged dir to a timestamped dir"""

    package_filename = build_package_basename(platform, timestamp)
    shutil.move(PACKAGING_DIR, package_filename)

    print("Packaged to directory %s" % os.path.abspath(package_filename))

def build_package_basename(platform, timestamp):
    """Build the name of our package and populate our JSON build info file"""

    # Fetch version from build info
    # TODO hardening, deal with: missing build info, exception loading build info file and no version entry
    with open(BUILDINFO_FILE) as json_file:
        build_info = json.load(json_file)

    package_filename = "NAP-%s-%s-%s" % (build_info['version'], platform, timestamp)
    return package_filename
    
if __name__ == '__main__':
    # TODO add options for
    # - not populating git revision into buildInfo

    parser = argparse.ArgumentParser()
    parser.add_argument("-nz", "--no-zip", action="store_true",
                        help="Don't zip the release, package to a directory")
    parser.add_argument("-c", "--clean", action="store_true",
                        help="Clean build")
    parser.add_argument("-a", "--include-apps", action="store_true",
                        help="Include Naivi apps, packaging them as projects")
    parser.add_argument("--include-docs", action="store_true",
                        help="Include documentation")      
    args = parser.parse_args()

    # Package our build
    package(not args.no_zip, args.include_docs, args.include_apps, args.clean)