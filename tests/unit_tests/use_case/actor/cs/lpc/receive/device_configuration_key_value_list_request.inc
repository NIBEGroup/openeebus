namespace cs_lpc_test {
namespace receive {

static constexpr char device_configuration_key_value_list_request[] = R"({
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
              "feature": 3
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
              "feature": 3
            }
          ]
        },
        {
          "msgCounter": 19
        },
        {
          "cmdClassifier": "read"
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "deviceConfigurationKeyValueListData": []
              }
            ]
          ]
        }
      ]
    }
  ]
})";

}  // namespace receive

}  // namespace cs_lpc_test
