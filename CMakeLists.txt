if(CONFIG_LCZ_LWM2M_TEMPERATURE)
    zephyr_include_directories(include)
    zephyr_sources(source/lcz_lwm2m_temperature.c)
endif()