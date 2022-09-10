cmake_minimum_required(VERSION 3.18.1)

# # 定义LIBS_DIR
set(LIBS_DIR ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI})
# 添加头文件目录
include_directories(src/main/cpp/include/)
#  添加ffmpeg相关的so库
# 添加库——外部引入的库
# 库名称：avcodec（不需要包含前缀lib）
# 库类型：SHARED，表示动态库，后缀为.so（如果是STATIC，则表示静态库，后缀为.a）
# IMPORTED表明是外部引入的库
add_library( avcodec
        SHARED
        IMPORTED)
# 设置目标属性
# 设置avcodec目标库的IMPORTED_LOCATION属性，用于说明引入库的位置
# 还可以设置其他属性，格式：PROPERTIES key value
set_target_properties( avcodec
        PROPERTIES IMPORTED_LOCATION
        ${LIBS_DIR}/libavcodec.so)

#add_library( avfilter
#        SHARED
#        IMPORTED)
#set_target_properties( avfilter
#        PROPERTIES IMPORTED_LOCATION
#        ${LIBS_DIR}/libavfilter.so)

add_library( avformat
        SHARED
        IMPORTED)
set_target_properties( avformat
        PROPERTIES IMPORTED_LOCATION
        ${LIBS_DIR}/libavformat.so)

add_library( avutil
        SHARED
        IMPORTED)
set_target_properties( avutil
        PROPERTIES IMPORTED_LOCATION
        ${LIBS_DIR}/libavutil.so)

add_library( swresample
        SHARED
        IMPORTED)
set_target_properties( swresample
        PROPERTIES IMPORTED_LOCATION
        ${LIBS_DIR}/libswresample.so)

add_library( swscale
        SHARED
        IMPORTED)
set_target_properties( swscale
        PROPERTIES IMPORTED_LOCATION
        ${LIBS_DIR}/libswscale.so)
# 查找代码中使用到的系统库
find_library(
        log-lib
        log )
# 配置目标so库编译信息
add_library(
        native-lib
        SHARED
        src/main/cpp/native-lib.cpp
)
# 指定编译目标库时，cmake要链接的库
target_link_libraries(
        # 指定目标库，native-lib 是在上面 add_library 中配置的目标库
        native-lib
        # 4. 连接 FFmpeg 相关的库
        avutil
        swresample
        avcodec
#        avfilter
        swscale
        avformat
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib} )