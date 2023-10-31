# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os
import json
import subprocess

swift_config_file = open('../version.json')
swift_config = json.load(swift_config_file)
swift_config_file.close()


# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'swift pilot client'
copyright = '2020, swift Project Community / Contributors'
author = 'swift Project Community / Contributors'

version = '.'.join([ str(swift_config['version']['major']),
                     str(swift_config['version']['minor']) ])
release = version


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = []

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_baseurl = 'https://apidocs.swift-project.org/'
html_copy_source = False
html_show_sourcelink = False


# -- Run Doxygen -------------------------------------------------------------

# https://stackoverflow.com/questions/36064976/using-doxygen-in-read-the-docs
html_extra_path = ['html']

env = {
    'DOXY_SRC_ROOT': '..'
}
subprocess.run(['doxygen', 'Doxyfile.qmake'], env=env, check=True)
