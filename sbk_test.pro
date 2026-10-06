TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += app

contains(CONFIG, build_tests) {
    SUBDIRS += tests
}
