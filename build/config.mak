# You can override those settings on the command line

PLATFORM ?= device
DEBUG ?= 0
DEVELOPMENT ?= 0

EPSILON_VERSION ?= 21.3.0
EXTERNAL_APPS_API_LEVEL ?= 0
EPSILON_APPS ?= calculation graph code statistics distributions inference solver sequence regression elements finance settings
EPSILON_I18N ?= en fr nl pt it de es
EPSILON_COUNTRIES ?= WW CA DE ES FR GB IT NL PT US
EPSILON_GETOPT ?= 0
ESCHER_LOG_EVENTS_BINARY ?= 0
ESCHER_LOG_EVENTS_NAME ?= $(DEBUG)
I18N_COMPRESS ?= 0
ASSERTIONS ?= $(DEBUG)
