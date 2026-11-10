namespace gcp_mgcp_test {
namespace send {

static constexpr char measurement_constraints_reply[] = R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
            {
              "device": "d:_n:GridConnectionPoint_123456789"
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
              "device": "d:_n:HEMS_123456789"
            },
            {
              "entity": [
                1
              ]
            },
            {
              "feature": 6
            }
          ]
        },
        {
          "msgCounter": 12
        },
        {
          "msgCounterReference": 10
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
                "measurementConstraintsListData": []
              }
            ]
          ]
        }
      ]
    }
  ]
})";

}  // namespace send
}  // namespace gcp_mgcp_test
