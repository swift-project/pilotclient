#include "sampleplugin.h"


void CXmlSamplePlugin::registerMetaTypes()
{
    qRegisterMetaType<BlackMiscTest::Dummy>("BlackMiscTest::Dummy");
    qDBusRegisterMetaType<BlackMiscTest::Dummy>();

    qRegisterMetaType<BlackMiscTest::DummyNoQ>("BlackMiscTest::DummyNoQ");
    qDBusRegisterMetaType<BlackMiscTest::DummyNoQ>();

}
