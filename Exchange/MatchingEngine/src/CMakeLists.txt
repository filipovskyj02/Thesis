add_executable(MatchingEngine main.cpp
        common/Order.h
        book/OrderBook.cpp
        book/OrderBook.h
        common/SideEnum.h
        common/CommonTypes.h
        common/OrderType.h
        core/MatchingEngine.cpp
        core/MatchingEngine.h
        core/EngineConfig.h
        common/SafeQueue.h
        io/dissemination/DisseminationEvent.h
        io/dissemination/Distributor.cpp
        io/dissemination/Distributor.h
        io/logging/Logger.cpp
        io/logging/Logger.h
        io/logging/LogEvent.h
        common/Utils.h
        io/kafka/reading/KafkaReader.cpp
        io/kafka/reading/KafkaReader.h
)

target_link_libraries(MatchingEngine
        PRIVATE
        modern-cpp-kafka-api
)
