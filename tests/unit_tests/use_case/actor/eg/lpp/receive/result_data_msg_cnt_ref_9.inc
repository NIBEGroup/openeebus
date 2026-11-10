namespace eg_lpp_test {
namespace receive {

static constexpr char result_data_msg_cnt_ref_9[] = R"({
  "datagram": [
    {
      "header": [
        {
          "specificationVersion": "1.3.0"
        },
        {
          "addressSource": [
            {
              "device": "d:_n:HeatPump_123456789"
            },
            {
              "entity": [
                0
              ]
            },
            {
              "feature": 0
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
                0
              ]
            },
            {
              "feature": 0
            }
          ]
        },
        {
          "msgCounter": 26
        },
        {
          "msgCounterReference": 9
        },
        {
          "cmdClassifier": "result"
        }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              {
                "resultData": [
                  {
                    "errorNumber": 0
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

}  // namespace receive
}  // namespace eg_lpp_test
