project(simple-socket-demo)
add_executable(server server.c intbuf.c) # 生成可执行文件
add_executable(client client.c intbuf.c) # 生成可执行文件
target_link_libraries(  # 目标库
                        server
 
                        # 目标库需要链接的库
                        pthread)
target_link_libraries(  # 目标库
                        client
 
                        # 目标库需要链接的库
                        pthread)