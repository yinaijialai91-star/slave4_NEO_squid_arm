/*右回転が正転*/
#include <driver/twai.h>
#include <SCServo.h>

SMS_STS st; // ID1, ID2はステッピングモード ID3, ID4は絶対角度モード

#define SLAVE1_WHEEL_CONTROL_ID 0x310    // タイヤ
#define SLAVE2_DISHES_ARM_ID 0x710       // お皿
#define SLAVE3_ZEUS_ARM_STS3215_ID 0x410 // 万能手腕
#define SLAVE4_SQUID_ARM_ID 0x110        // いかさん
#define SLAVE5_MARKER_ARM_ID 0x210       // マーカー
#define SLAVEX_BUTSUDAN_LED_ID 0x115     // 仏壇

#define TWAI_TX_PIN D0
#define TWAI_RX_PIN D1

#define SERVO_TX_PIN D2
#define SERVO_RX_PIN D3

#define ARM_SPEED 3000
#define SERVO_FIRST 2047

/***********HAND_DEFINE***********/
#define HAND_ID_1_TSUKAMU 2047
#define HAND_ID_1_HANASU 2650

#define HAND_ID_2_TSUKAMU 2047
#define HAND_ID_2_HANASU 2650

#define HAND_ID_3_TSUKAMU 2047
#define HAND_ID_3_HANASU 2650

#define HAND_ID_4_TSUKAMU 2047
#define HAND_ID_4_HANASU 2650

/***********HAND_DEFINE***********/

/***********ARM_DEFINE***********/

#define ARM_ID_1_TSUKAMU 3100
#define ARM_ID_1_HANASU 2047

#define ARM_ID_2_TSUKAMU 3060
#define ARM_ID_2_HANASU 2047

/***********ARM_DEFINE***********/

/***********DAKIMAKURA_DEFINE***********/

#define DAKIMAKURA_ID_1_DAKITSUKU 0
#define DAKIMAKURA_ID_1_KYOZETSU 4090

#define DAKIMAKURA_ID_2_DAKITSUKU 0
#define DAKIMAKURA_ID_2_KYOZETSU 4090

/***********DAKIMAKURA_DEFINE***********/

/***********HURIHARAUYATU_DEFINE***********/

#define HURIHARAUYATSU_START 4000
#define HURIHARAUYATSU_HURIHARAU_CENTER 2980
#define HURIHARAUYATSU_HURIHARAU_OKUMADE 1940
#define HURIHARAUYATSU_HURIHARAU_SOREPPOINO 2700

/***********HURIHARAUYATU_DEFINE***********/

/***********MARKER_DEFINE***********/

#define MARKER_TOBASU 11000

/***********MARKER_DEFINE***********/

uint16_t ID = 0;
int8_t DATA[8] = {0};
twai_message_t receiveframe;

void motor_control()
{

  switch (DATA[0])
  {
  case 1: // アームくるくる
    switch (DATA[1])
    {
    case 1: // 倒す
      st.RegWritePosEx(7, ARM_ID_1_TSUKAMU, ARM_SPEED);
      st.RegWritePosEx(8, ARM_ID_2_TSUKAMU, ARM_SPEED);
      st.RegWriteAction();
      break;

    case 2: // 立てる
      st.RegWritePosEx(7, ARM_ID_1_HANASU, ARM_SPEED);
      st.RegWritePosEx(8, ARM_ID_2_HANASU, ARM_SPEED);
      st.RegWriteAction();
      break;

    case 3: // 皿置くとき専用
      vTaskDelay(pdMS_TO_TICKS(5000));
      st.WritePosEx(8, ARM_ID_1_TSUKAMU, ARM_SPEED);
      break;

    default:
      break;
    }
    break;

  case 2: // アーム関節
    switch (DATA[1])
    {
    case 1: // 抱きつく
      st.RegWritePosEx(5, DAKIMAKURA_ID_1_DAKITSUKU, ARM_SPEED);
      st.RegWritePosEx(6, DAKIMAKURA_ID_2_DAKITSUKU, ARM_SPEED);
      st.RegWriteAction();
      break;

    case 2: // 拒絶する
      st.RegWritePosEx(5, DAKIMAKURA_ID_1_KYOZETSU, ARM_SPEED);
      st.RegWritePosEx(6, DAKIMAKURA_ID_2_KYOZETSU, ARM_SPEED);
      st.RegWriteAction();
      break;

    default:
      break;
    }
    break;

  case 3: // ハンド開閉
    switch (DATA[1])
    {
    case 1:
      st.RegWritePosEx(1, HAND_ID_1_HANASU, ARM_SPEED);
      st.RegWritePosEx(2, HAND_ID_2_HANASU, ARM_SPEED);
      st.RegWritePosEx(3, HAND_ID_3_HANASU, ARM_SPEED);
      st.RegWritePosEx(4, HAND_ID_4_HANASU, ARM_SPEED);
      st.RegWriteAction();
      break;

    case 2:
      st.RegWritePosEx(1, HAND_ID_1_TSUKAMU, ARM_SPEED);
      st.RegWritePosEx(2, HAND_ID_2_TSUKAMU, ARM_SPEED);
      st.RegWritePosEx(3, HAND_ID_3_TSUKAMU, ARM_SPEED);
      st.RegWritePosEx(4, HAND_ID_4_TSUKAMU, ARM_SPEED);
      st.RegWriteAction();
      break;

    case 3:
      st.WritePosEx(3, HAND_ID_3_HANASU, ARM_SPEED);
      break;

    case 4:
      st.WritePosEx(1, HAND_ID_1_HANASU, ARM_SPEED);
      break;

    case 5:
      st.WritePosEx(2, HAND_ID_2_HANASU, ARM_SPEED);
      break;

    case 6:
      st.WritePosEx(4, HAND_ID_4_HANASU, ARM_SPEED);
      break;

    default:
      break;
    }
    break;

  case 4: // 釣り竿伸びー
    switch (DATA[1])
    {
    case 1:
      st.WritePosEx(9, MARKER_TOBASU, 3400);
      break;

    case 2:
      st.WritePosEx(9, 200, 3400);
      break;

    case 3:
      st.WritePosEx(9, -200, 3400);
      break;

    default:
      break;
    }
    break;

  case 5:
    switch (DATA[1])
    {
    case 1:
      st.WritePosEx(10, HURIHARAUYATSU_START, 3000);
      break;

    case 2:
      st.WritePosEx(10, HURIHARAUYATSU_HURIHARAU_SOREPPOINO, 2000);
      vTaskDelay(pdMS_TO_TICKS(1600));
      st.WritePosEx(10, HURIHARAUYATSU_HURIHARAU_OKUMADE, 3000);
      break;
    }
    break;

  default:
    break;
  }
}

void setup()
{

  Serial.begin(115200);

  vTaskDelay(pdMS_TO_TICKS(200));

  /***********************************CAN関連********************************************/
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)TWAI_TX_PIN, (gpio_num_t)TWAI_RX_PIN, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  esp_err_t ret = twai_driver_install(&g_config, &t_config, &f_config);
  if (ret == ESP_OK)
    Serial.printf("インストール完了\n");
  else
    Serial.printf("インストール失敗\n");
  ret = twai_start();
  if (ret == ESP_OK)
    Serial.printf("CANスタート完了\n");
  else
    Serial.printf("CANスタート失敗\n");
  /**************************************************************************************/

  /***********************************サーボ関連*****************************************/

  bool first_init = false;

  do
  {
    if (first_init)
    {
      Serial.printf("STS Serial setup fail\n一秒後に再試行します\n");
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
    Serial1.begin(1000000, SERIAL_8N1, SERVO_RX_PIN, SERVO_TX_PIN);
    st.pSerial = &Serial1;
    first_init = true;
  } while (!Serial1);

  for (int i = 1; i <= 8; i++)
  {
    st.EnableTorque(i, 1);
    st.RegWritePosEx(i, SERVO_FIRST, ARM_SPEED);
    vTaskDelay(pdMS_TO_TICKS(10));
  }

  st.RegWriteAction();

  Serial.printf("STS Serial1 setup succes\n");

  /**************************************************************************************/
}

void loop()
{

  if (twai_receive(&receiveframe, portMAX_DELAY) == ESP_OK)
  {
    if (receiveframe.identifier == SLAVE4_SQUID_ARM_ID)
    {

      ID = receiveframe.identifier;
      DATA[0] = receiveframe.data[0]; // 命令の識別子
      DATA[1] = receiveframe.data[1]; // 命令の数値(足回りの場合は倍率に使用)
      DATA[2] = receiveframe.data[2]; // vector1
      DATA[3] = receiveframe.data[3]; // vector2M
      DATA[4] = receiveframe.data[4]; // vector3
      DATA[5] = receiveframe.data[5]; // vector4
      DATA[6] = receiveframe.data[6]; // ゴミ
      DATA[7] = receiveframe.data[7]; // ゴミ
      Serial.printf("ID = %x, data1 = %d, data2 = %d, data3 = %d, data4 = %d, data5 = %d, data6 = %d, data7 = %d, data8 = %d\n", receiveframe.identifier, DATA[0], DATA[1], DATA[2], DATA[3], DATA[4], DATA[5], DATA[6], DATA[7]);

      motor_control();
    }
  }

  vTaskDelay(pdMS_TO_TICKS(1));
}