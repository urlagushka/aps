cmake ./cv_service/CMakeLists.txt
make ./cv_service/Makefile -j8

cmake ./pg_service/CMakeLists.txt
make ./pg_service/Makefile -j8
