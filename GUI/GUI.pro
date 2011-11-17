#-------------------------------------------------
#
# Project created by QtCreator 2011-10-20T18:04:36
#
#-------------------------------------------------

QT       += core webkit network
CONFIG += assistant

TARGET = GUI

TEMPLATE = app

INCLUDEPATH += ../ ../DB

#DEPENDPATH += ../ ../DB
#DEFINES += NO_PG
#DEFINES += NO_SQLITE
#DEFINES += WITH_PGSQL

DEPENDPATH += ../



LIBS += -L/usr/lib/ -lpq
LIBS += -L/usr/lib/ -lsqlite3
LIBS += -L../DB -lDB

HEADERS += \
    main.h \
    login.h \
    mail_displayer.h \
    app_config.h \
    addresses.h \
    addressbook.h \
    about.h \
    browser.h \
    body_view.h \
    body_edit.h \
    bitvector.h \
    attachment_listview.h \
    attachment.h \
    filter_rules.h \
    filter_log.h \
    errors.h \
    edit_rules.h \
    edit_address_widget.h \
    dragdrop.h \
    mail_listview.h \
    mailheader.h \
    identities.h \
    icons.h \
    html_editor.h \
    helper.h \
    headers_view.h \
    headers_groupview.h \
    words.h \
    users.h \
    user_queries.h \
    ui_controls.h \
    tagsdialog.h \
    tagsbox.h \
    tags.h \
    sql_editor.h \
    sha1.h \
    selectmail.h \
    searchbox.h \
    query_listview.h \
    prog_chooser.h \
    preferences.h \
    notewidget.h \
    notepad.h \
    newmailwidget.h \
    mygetopt.h \
    msg_status_cache.h \
    msgs_page_list.h \
    msg_properties.h \
    msg_list_window.h \
    mime_msg_viewer.h \
    message_view.h \
    message_port.h \
    message.h \
    xface/xface.h \
    xface/vars.h \
    xface/data.h \
    xface/compface.h \
    ../config.h

SOURCES += \
    main.cpp \
    login.cpp \
    mail_displayer.cpp \
    app_config.cpp \
    addresses.cpp \
    addressbook.cpp \
    about.cpp \
    browser.cpp \
    body_view.cpp \
    body_edit.cpp \
    bitvector.cpp \
    attachment_listview.cpp \
    attachment.cpp \
    getopt1.cpp \
    getopt.cpp \
    filter_rules.cpp \
    filter_log.cpp \
    errors.cpp \
    edit_rules.cpp \
    edit_address_widget.cpp \
    mail_listview.cpp \
    mailheader.cpp \
    identities.cpp \
    html_editor.cpp \
    helper.cpp \
    headers_view.cpp \
    headers_groupview.cpp \
    words.cpp \
    users.cpp \
    user_queries.cpp \
    tagsdialog.cpp \
    tagsbox.cpp \
    tags.cpp \
    sql_editor.cpp \
    sha1.cpp \
    selectmail.cpp \
    searchbox.cpp \
    query_listview.cpp \
    prog_chooser.cpp \
    preferences.cpp \
    notewidget.cpp \
    notepad.cpp \
    newmailwidget.cpp \
    msg_status_cache.cpp \
    msgs_page_list.cpp \
    msg_properties.cpp \
    msg_list_window_pages.cpp \
    msg_list_window.cpp \
    mime_msg_viewer.cpp \
    message_view.cpp \
    message_port.cpp \
    message.cpp \
    xface/xface.cpp \
    xface/gen.cpp \
    xface/file.cpp \
    xface/compress.cpp \
    xface/arith.cpp
