#include <driver/twai.h>
#include <SCServo.h>

SMS_STS st;  //ID1, ID2はステッピングモード ID3, ID4は絶対角度モード
SCSCL sc;

#define STS_MODE 10
#define SCS_MODE 20

#define NORMAL_RECEIVE_ID 0x110

#define TWAI_TX_PIN D0
#define TWAI_RX_PIN D1

#define TX_PIN D2
#define RX_PIN D3

#define HAND_ID_1_TSUKAMU 575
#define HAND_ID_1_HANASU 500

#define HAND_ID_2_TSUKAMU 80
#define HAND_ID_2_HANASU 150

#define ARM_ID_1_FIRST -9000
#define ARM_ID_1_SECOND 9000

#define ARM_ID_2_FIRST 9000
#define ARM_ID_2_SECOND -9000

uint16_t ID = 0;
int8_t data[8] = { 0 };
twai_message_t receiveframe;




int SERVO_MODE = SCS_MODE;




void motor_control() {

  switch (data[0]) {
    case 1:  //棒伸ばす
      if (SERVO_MODE == STS_MODE) {
        st.RegWritePosEx(1, ARM_ID_1_FIRST, 3400);
        st.RegWritePosEx(2, ARM_ID_2_FIRST, 3400);
        st.RegWriteAction();
        Serial.printf("1\n");
      }
      break;

    case 2:
      if (SERVO_MODE == SCS_MODE) {
        sc.RegWritePos(1, HAND_ID_1_TSUKAMU, 50);
        sc.RegWritePos(2, HAND_ID_2_TSUKAMU, 50);
        sc.RegWriteAction();
        Serial.printf("2\n");
      }
      break;

    case 3:
      if (SERVO_MODE == STS_MODE) {
        st.RegWritePosEx(1, ARM_ID_1_SECOND, 3400);
        st.RegWritePosEx(2, ARM_ID_2_SECOND, 3400);
        st.RegWriteAction();
        Serial.printf("3\n");
      }
      break;

    case 4:
      if (SERVO_MODE == SCS_MODE) {
        sc.WritePosEx(1, HAND_ID_1_HANASU, 3400, 50);
        Serial.printf("4\n");
      }
      break;

    case 5:
      if (SERVO_MODE == SCS_MODE) {
        sc.WritePosEx(2, HAND_ID_2_HANASU, 3400, 50);
        Serial.printf("5\n");
      }
      break;

    default:
      break;
  }
}

void setup() {

  Serial.begin(115200);

  vTaskDelay(pdMS_TO_TICKS(200));

  /***********************************CAN関連********************************************/
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)TWAI_TX_PIN, (gpio_num_t)TWAI_RX_PIN, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  esp_err_t ret = twai_driver_install(&g_config, &t_config, &f_config);
  if (ret == ESP_OK) Serial.println("インストール完了");
  else Serial.println("インストール失敗");
  ret = twai_start();
  if (ret == ESP_OK) Serial.println("CANスタート完了");
  else Serial.println("CANスタート失敗");
  /**************************************************************************************/

  /***********************************サーボ関連*****************************************/

  if (SERVO_MODE == STS_MODE) {
    Serial1.begin(1000000, SERIAL_8N1, RX_PIN, TX_PIN);
    st.pSerial = &Serial1;
    if (Serial1) {
      Serial.printf("STS Serial1 setup succes\n");
    }
  } else if (SERVO_MODE == SCS_MODE) {
    Serial1.begin(1000000, SERIAL_8N1, RX_PIN, TX_PIN);
    sc.pSerial = &Serial1;
    if (Serial1) {
      Serial.printf("SCS Serial1 setup succes\n");
    }
  }

  /**************************************************************************************/
}

void loop() {

  if (twai_receive(&receiveframe, portMAX_DELAY) == ESP_OK) {
    if (receiveframe.identifier == NORMAL_RECEIVE_ID) {

      ID = receiveframe.identifier;
      data[0] = receiveframe.data[0];  //命令の識別子
      data[1] = receiveframe.data[1];  //命令の数値(足回りの場合は倍率に使用)
      data[2] = receiveframe.data[2];  //vector1
      data[3] = receiveframe.data[3];  //vector2M
      data[4] = receiveframe.data[4];  //vector3
      data[5] = receiveframe.data[5];  //vector4
      data[6] = receiveframe.data[6];  //ゴミ
      data[7] = receiveframe.data[7];  //ゴミ
      Serial.printf("ID = %x, data1 = %d, data2 = %d, data3 = %d, data4 = %d, data5 = %d, data6 = %d, data7 = %d, data8 = %d\n", receiveframe.identifier, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

      motor_control();
    }
  }

  vTaskDelay(pdMS_TO_TICKS(1));
}
