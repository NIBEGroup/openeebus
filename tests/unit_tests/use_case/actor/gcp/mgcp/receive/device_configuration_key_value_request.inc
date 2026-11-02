namespace gcp_mgcp_test {
namespace receive {

static constexpr char device_configuration_key_value_request[] = R"({
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
              "device": "d:_n:GridConnectionPoint_123456789"
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
          "msgCounter": 13
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
}  // namespace gcp_mgcp_test
