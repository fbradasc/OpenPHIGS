ADD_EXECUTABLE(view_ori view_ori.c ../xbasic/openxws.c)
ADD_EXECUTABLE(pvt pvt.c ../xbasic/openxws.c)
ADD_EXECUTABLE(pvt2 pvt2.c ../xbasic/openxws.c)
ADD_EXECUTABLE(ride ride.c ../xbasic/openxws.c)
ADD_EXECUTABLE(twod_view twod_view.c ../xbasic/openxws.c)

TARGET_LINK_LIBRARIES(view_ori ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(pvt ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(pvt2 ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(ride ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(twod_view ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            view_ori
            pvt
            pvt2
            ride
            twod_view
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
