namespace eg_lpc_test {
namespace send {

static constexpr char device_diagnosis_heartbeat_reply[] = R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
            {
              "device": "d:_n:HEMS_123456789"
            },
            {
              "entity": [
                1
              ]
            },
            {
              "feature": 5
            }
          ]
        },
        {
          "addressDestination": [
            {
              "device": "d:_n:HeatPump_123456789"
            },
            {
              "entity": [
                1
              ]
            },
            {
              "feature": 1
            }
          ]
        },
        {
          "msgCounter": 18
        },
        {
          "msgCounterReference": 32
        },
        {
          "cmdClassifier": "reply"
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "deviceDiagnosisHeartbeatData": [
                  {
                    "timestamp": "2026-04-07T15:37:54Z"
                  },
                  {
                    "heartbeatCounter": 0
                  },
                  {
                    "heartbeatTimeout": "PT60S"
                  }
                ]
              }
            ]
          ]
        }
      ]
    }
  ]
})";

}  // namespace send
}  // namespace eg_lpc_test
