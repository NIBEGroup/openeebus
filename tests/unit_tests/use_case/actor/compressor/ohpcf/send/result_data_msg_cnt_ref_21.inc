namespace compressor_ohpcf_test {
namespace send {

static constexpr char result_data_msg_cnt_ref_21[] = R"({
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
                1,
                1
              ]
            },
            {
              "feature": 1
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
              "feature": 0
            }
          ]
        },
        {
          "msgCounter": 28
        },
        {
          "msgCounterReference": 21
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

}  // namespace send
}  // namespace compressor_ohpcf_test
