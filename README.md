# DualSense Windows DLL

Windows専用のDualSenseコントローラー制御ライブラリです。C-style APIを提供し、LED制御、振動、アダプティブトリガーエフェクト、オーディオハプティクスをサポートします。

ほとんどのコードは、ありがたき先駆者 https://github.com/rafaelvaloto/WindowsDualsenseUnreal から借用しました。元の実装は Unreal 専用のプラグインでしたが、そこから Unreal の依存を外し、 C-style の dll にコンパイルできるように独自実装を追加したものです。

## 特徴

- Cスタイルの呼び出し規約を採用しているため、dll呼び出しをサポートする全てのプログラミング言語で利用できるはずです。
- WindowsのAPIを直接たたいているため、他のプラットフォームでは動きませんが、dllサイズとして小さく済んでいます。
- ほとんどの機能をサポート: PS5 コントローラーが持つほぼ全ての機能をサポートしています。タッチパッドはこれから対応予定。ジャイロは今のところ対応予定がありません。
- USBでもBluetoothでも使用できる……と思います。

## 必要環境

- Windows 10/11
- MSVC++ (Visual Studio 2019以降)
- nmake
- DualSenseまたはDualSense Edgeコントローラー

## ビルド方法

### 1. Developer Command Promptを開く

Visual Studioの「Developer Command Prompt」または「Developer PowerShell」を起動します。

### 2. DLLをビルド

```cmd
nmake
```

ビルドが成功すると、`bin/dualsense.dll`と`bin/dualsense.lib`が生成されます。

### 3. サンプルプログラムをビルド

```cmd
cd samples\basic_test
nmake
```

### 4. 実行

DualSenseコントローラーを接続してから：

```cmd
nmake run
```

または

```cmd
basic_test.exe
```

## クリーンアップ

```cmd
nmake clean
```

## API使用例

### 基本的な使い方

```c
#include <dualsense.h>
#include <stdio.h>

int main() {
    // デバイス初期化
    if (ds_init() != DS_OK) {
        printf("Failed to initialize\n");
        return 1;
    }

    // ライトバーを赤に設定
    ds_set_lightbar(255, 0, 0);

    // 両方のモーターで振動
    ds_set_rumble(128, 128);

    // ボウエフェクトを設定
    ds_trigger_bow(true, true, 2, 7, 4, 8);

    // 3秒待つ
    Sleep(3000);

    // クリーンアップ
    ds_reset_all();
    ds_shutdown();

    return 0;
}
```

### LED制御

```c
// ライトバーの色を設定（RGB 0-255）
ds_set_lightbar(255, 128, 0);  // オレンジ

// プレイヤーLED
ds_set_player_led(DS_LED_PLAYER_1, DS_LED_BRIGHTNESS_HIGH);

// マイクLED
ds_set_mic_led(DS_LED_MIC_ON);  // マイクオン
```

### 振動制御

```c
// 左右のモーター（0-255）
ds_set_rumble(200, 200);

// 停止
ds_stop_rumble();
```

### アダプティブトリガー

```c
// ボウエフェクト（引き絞る感覚）
ds_trigger_bow(true, true, 2, 7, 4, 8);

// ウェポンエフェクト（セクション別抵抗）
ds_trigger_weapon(true, true, 3, 8, 7);

// オートマチックガン（連射）
ds_trigger_automatic_gun(true, true, 3, 6, 200);

// マシン（振動）
ds_trigger_machine(true, true, 3, 5, 150);

// エフェクトを停止
ds_trigger_off(true, true);

// カスタムエフェクト（生パラメータ）
uint8_t params[10] = { /* ... */ };
ds_trigger_custom(true, false, params);  // 右トリガーのみ
```

### 入力読み取り

```c
DSInputState state;

while (running) {
    // 入力を更新
    ds_update_input();

    // 状態を取得
    if (ds_get_input_state(&state) == DS_OK) {
        if (state.button_cross) {
            printf("Cross button pressed\n");
        }

        printf("Left stick: %d, %d\n", state.stick_lx, state.stick_ly);
        printf("Battery: %d%%\n", state.battery_level);
    }

    Sleep(16);  // ~60Hz
}
```

## API リファレンス

### デバイス管理

| 関数 | 説明 |
|------|------|
| `ds_init()` | デバイスを初期化して接続 |
| `ds_shutdown()` | デバイスを切断 |
| `ds_is_connected()` | 接続状態を確認 |
| `ds_get_connection_type()` | USB/Bluetoothを取得 |
| `ds_get_device_type()` | DualSense/Edgeを取得 |

### LED制御

| 関数 | 説明 |
|------|------|
| `ds_set_lightbar(r, g, b)` | ライトバーの色を設定 |
| `ds_set_player_led(led, brightness)` | プレイヤーLEDを設定 |
| `ds_set_mic_led(mode)` | マイクLEDを設定 |

### 振動

| 関数 | 説明 |
|------|------|
| `ds_set_rumble(left, right)` | 振動の強さを設定（0-255） |
| `ds_stop_rumble()` | 振動を停止 |

### トリガーエフェクト

| 関数 | モード | 説明 |
|------|--------|------|
| `ds_trigger_off()` | 0x00 | エフェクトを停止 |
| `ds_trigger_continuous_resistance()` | 0x01 | 連続抵抗 |
| `ds_trigger_bow()` | 0x02 | ボウ（弓） |
| `ds_trigger_resistance()` | 0x21 | セクション別抵抗 |
| `ds_trigger_galloping()` | 0x23 | ギャロッピング（リズム） |
| `ds_trigger_weapon()` | 0x25 | ウェポン |
| `ds_trigger_automatic_gun()` | 0x26 | オートマチックガン |
| `ds_trigger_machine()` | 0x27 | マシン（振動） |
| `ds_trigger_custom()` | 0xFF | カスタム（生パラメータ） |

### オーディオハプティクス

| 関数 | 説明 |
|------|------|
| `ds_send_audio_haptic(data, size)` | オーディオハプティクスデータを送信（Bluetoothのみ） |

### ユーティリティ

| 関数 | 説明 |
|------|------|
| `ds_reset_all()` | 全エフェクトをリセット |
| `ds_flush_output()` | 出力を即座に送信 |

## エラーコード

| コード | 値 | 説明 |
|--------|-----|------|
| `DS_OK` | 0 | 成功 |
| `DS_ERROR_NOT_FOUND` | -1 | デバイスが見つからない |
| `DS_ERROR_ALREADY_CONNECTED` | -2 | 既に接続済み |
| `DS_ERROR_NOT_CONNECTED` | -3 | 未接続 |
| `DS_ERROR_INVALID_PARAM` | -4 | 無効なパラメータ |
| `DS_ERROR_IO_FAILED` | -5 | I/O失敗 |

## プロジェクト構造

```
cpp/
├── include/
│   └── dualsense.h              # パブリックAPI
├── src/
│   ├── api/                     # C API実装
│   ├── core/                    # コアデータ構造
│   ├── hid/                     # Windows HID通信
│   ├── protocol/                # DualSenseプロトコル
│   └── dllmain.cpp
├── samples/
│   └── basic_test/              # サンプルプログラム
├── Makefile
└── README.md
```

## ライセンス

元のプロジェクトと同じライセンスに従います。

## 参考資料

- 元のUnreal Engineプラグイン: WindowsDualsense_ds5w
- DualSenseプロトコル仕様: コミュニティリサーチに基づく

## トラブルシューティング

### デバイスが見つからない

1. DualSenseがUSBまたはBluetoothで接続されているか確認
2. Windowsがデバイスを認識しているか（デバイスマネージャーで確認）
3. 他のアプリ（Steam等）がコントローラーを占有していないか

### ビルドエラー

1. Visual Studio Developer Command Promptを使用しているか確認
2. MSVC++がインストールされているか確認
3. パスに空白が含まれている場合は、短いパスに移動

### 実行時エラー

1. `dualsense.dll`が実行ファイルと同じディレクトリにあるか確認
2. Visual C++ Redistributableがインストールされているか確認

## 制限事項

- 単一デバイスのみサポート（複数コントローラー非対応）
- Windows専用（Linux/Mac非対応）
- 入力パース機能は最小限の実装（拡張予定）

## 今後の拡張

- 完全な入力パース実装
- タッチパッドサポート
- ジャイロスコープ/加速度計のサポート
- 複数デバイスのサポート
