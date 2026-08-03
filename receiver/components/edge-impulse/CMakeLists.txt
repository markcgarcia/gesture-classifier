idf_component_register(
    SRC_DIRS
        # Classifier core
        "edge-impulse-sdk/classifier"
        "edge-impulse-sdk/classifier/inferencing_engines"
        "edge-impulse-sdk/classifier/postprocessing"

        # DSP (feature extraction)
        "edge-impulse-sdk/dsp"
        "edge-impulse-sdk/dsp/dct"
        "edge-impulse-sdk/dsp/kissfft"
        "edge-impulse-sdk/dsp/spectral"
        "edge-impulse-sdk/dsp/speechpy"

        # Espressif-specific porting layer (esp-dsp + ESP-NN)
        "edge-impulse-sdk/porting/espressif"
        "edge-impulse-sdk/porting/espressif/esp-dsp/modules/common"
        "edge-impulse-sdk/porting/espressif/esp-dsp/modules/fft/fixed"
        "edge-impulse-sdk/porting/espressif/esp-dsp/modules/fft/float"

        # TensorFlow Lite Micro core + kernels
        "edge-impulse-sdk/tensorflow/lite/c"
        "edge-impulse-sdk/tensorflow/lite/core"
        "edge-impulse-sdk/tensorflow/lite/core/api"
        "edge-impulse-sdk/tensorflow/lite/core/c"
        "edge-impulse-sdk/tensorflow/lite/kernels"
        "edge-impulse-sdk/tensorflow/lite/kernels/internal"
        "edge-impulse-sdk/tensorflow/lite/kernels/internal/reference"
        "edge-impulse-sdk/tensorflow/lite/micro"
        "edge-impulse-sdk/tensorflow/lite/micro/kernels"
        "edge-impulse-sdk/tensorflow/lite/micro/memory_planner"

        # Third-party deps TFLite Micro needs
        "edge-impulse-sdk/third_party/flatbuffers/include"
        "edge-impulse-sdk/third_party/gemmlowp"
        "edge-impulse-sdk/third_party/ruy"

        # Your model + config
        "tflite-model"
        "model-parameters"

    INCLUDE_DIRS
        "."
        "edge-impulse-sdk"
        "edge-impulse-sdk/third_party"
        "edge-impulse-sdk/third_party/ruy"
        "edge-impulse-sdk/third_party/gemmlowp"
        "edge-impulse-sdk/third_party/flatbuffers/include"
        "edge-impulse-sdk/third_party/flatbuffers/include/flatbuffers"
        "edge-impulse-sdk/CMSIS/NN/Include"
        "edge-impulse-sdk/CMSIS/DSP/Include"
        "edge-impulse-sdk/CMSIS/DSP/PrivateInclude"
        "edge-impulse-sdk/CMSIS/Core/Include"
        "edge-impulse-sdk/porting/espressif/esp-dsp/modules/common/include"
        "edge-impulse-sdk/porting/espressif/esp-dsp/modules/fft/include"
        "model-parameters"
        "tflite-model"

    REQUIRES
        esp_timer
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -DEI_CLASSIFIER_TFLITE_ENABLE_ESP_NN=0
    -DTF_LITE_STATIC_MEMORY
    -DTF_LITE_DISABLE_X86_NEON
)