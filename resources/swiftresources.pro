load(common_pre)

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

OTHER_FILES += data/images/flags/*.png
OTHER_FILES += data/images/airlines/*.png
OTHER_FILES += swiftDB/*.*
OTHER_FILES += bootstrap/productive/*.*
OTHER_FILES += bootstrap/development/*.*
OTHER_FILES += local.env.template/*.*
OTHER_FILES += local.env.template/bootstrap/*.*
OTHER_FILES += local.env.template/bootstrap/0.6/*.*

COPY_FILES += $$PWD/data/images/flags/*.png
COPY_FILES += $$PWD/data/images/airlines/*.png
COPY_FILES += $$PWD/swiftDB/*.*
COPY_FILES += $$PWD/bootstrap/productive/*.*
COPY_FILES += $$PWD/bootstrap/development/*.*
COPY_FILES += $$PWD/local.env.template/*.*
COPY_FILES += $$PWD/local.env.template/bootstrap/*.*
COPY_FILES += $$PWD/local.env.template/bootstrap/0.6/*.*

package_resources.path = $$PREFIX
package_resources.files += data
package_resources.files += swiftDB
package_resources.files += bootstrap
package_resources.files += local.env.template
INSTALLS += package_resources

load(common_post)
