# Dart server to submit results (used by client)
SET (DROP_SITE "public.kitware.com")
SET (DROP_LOCATION "/incoming")
SET (DROP_SITE_USER "ftpuser")
SET (DROP_SITE_PASSWORD "public")
SET (TRIGGER_SITE 
       "http://${DROP_SITE}/cgi-bin/Submit-Insight-TestingResults.pl")

# Dart server configuration 
SET (CVS_WEB_URL "http://${DROP_SITE}/cgi-bin/itkcvsweb.cgi/Insight/")
SET (CVS_WEB_CVSROOT "Insight")

OPTION(BUILD_DOXYGEN "Build source documentation using doxygen" "On")
SET (DOXYGEN_CONFIG "${PROJECT_BINARY_DIR}/doxygen.config" )
SET (USE_DOXYGEN "On")
SET (DOXYGEN_URL "http://${DROP_SITE}/Insight/Doxygen/html/" )

SET (USE_GNATS "On")
SET (GNATS_WEB_URL "http://${DROP_SITE}/cgi-bin/gnatsweb.pl/Insight/")
