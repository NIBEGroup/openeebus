namespace eg_lpp_test {
namespace receive {

static constexpr char result_data_msg_cnt_ref_23[] = R"({
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
          "msgCounter": 63
        },
        {
          "msgCounterReference": 23
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
