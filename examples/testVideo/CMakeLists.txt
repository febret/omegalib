#######################################################################################################################
# Source files
SET( srcs 
		libstreamport.cpp
		video_buf_pool.cpp
		video_ctrl.cpp
		video_stream_manager.cpp
		video_stream_port.cpp
        main.cpp
        )
    
#######################################################################################################################
# Headers
SET( headers 
        libstreamport.h
		video_buf_pool.h
		video_ctrl.h
		video_stream_manager.h
		video_stream_port.h
		) 
        
#######################################################################################################################
# Setup compile info
add_executable(testVideo ${srcs} ${headers})
set_target_properties(testVideo PROPERTIES FOLDER examples)
target_link_libraries(testVideo omega omegaToolkit)


