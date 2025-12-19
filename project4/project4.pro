TEMPLATE = subdirs

SUBDIRS += \
    ClientApp \
    ServerApp \

client.depends = common
server.depends = common
