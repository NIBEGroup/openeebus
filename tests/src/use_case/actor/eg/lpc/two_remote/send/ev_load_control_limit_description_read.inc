namespace two_remote_eg_lpc_test {
namespace send {

static constexpr char load_control_limit_description_read[] = R"({
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
              "feature": 2
            }
          ]
        },
        {
          "addressDestination": [
            {
              "device": "d:_n:EV_123456789"
            },
            {
              "entity": [
                1
              ]
            },
            {
              "feature": 2
            }
          ]
        },
        {
          "msgCounter": 9
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
                "loadControlLimitDescriptionListData": []
              }
            ]
          ]
        }
      ]
    }
  ]
})";

}  // namespace send
}  // namespace two_remote_eg_lpc_test
