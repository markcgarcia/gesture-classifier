
# 1. Gather core Edge Impulse sources (Now including .cc files for TensorFlow)
file(GLOB_RECURSE ei_sources 
    "tflite-model/*.cpp" "tflite-model/*.c" "tflite-model/*.cc"
    "edge-impulse-sdk/classifier/*.cpp" "edge-impulse-sdk/classifier/*.c" "edge-impulse-sdk/classifier/*.cc"
    "edge-impulse-sdk/dsp/*.cpp" "edge-impulse-sdk/dsp/*.c" "edge-impulse-sdk/dsp/*.cc"
    "edge-impulse-sdk/tensorflow/*.cpp" "edge-impulse-sdk/tensorflow/*.c" "edge-impulse-sdk/tensorflow/*.cc"
    "edge-impulse-sdk/CMSIS/*.cpp" "edge-impulse-sdk/CMSIS/*.c" "edge-impulse-sdk/CMSIS/*.cc"
)

# 2. Gather Espressif-specific porting and ESP-DSP sources recursively (Now including Assembly .S/.s files)
file(GLOB_RECURSE ei_porting_sources
    "edge-impulse-sdk/porting/espressif/*.cpp" 
    "edge-impulse-sdk/porting/espressif/*.c" 
    "edge-impulse-sdk/porting/espressif/*.cc"
    "edge-impulse-sdk/porting/espressif/*.S"
    "edge-impulse-sdk/porting/espressif/*.s"
)

# 3. Gather base porting files (Non-recursive so we don't accidentally compile code for other chips)
file(GLOB ei_base_porting_sources
    "edge-impulse-sdk/porting/*.cpp" 
    "edge-impulse-sdk/porting/*.c" 
    "edge-impulse-sdk/porting/*.cc"
)

# Combine all the sources we found
list(APPEND ei_sources ${ei_porting_sources} ${ei_base_porting_sources})

# Register component
idf_component_register(
    SRCS ${ei_sources}
    INCLUDE_DIRS 
        "."
        "model-parameters"
        "tflite-model"
        "edge-impulse-sdk"
        "edge-impulse-sdk/classifier"
        "edge-impulse-sdk/porting"
        "edge-impulse-sdk/dsp"
        "edge-impulse-sdk/third_party/flatbuffers/include"
        "edge-impulse-sdk/third_party/gemmlowp"
        "edge-impulse-sdk/third_party/ruy"
        # --- ESP-DSP module directories ---
        "edge-impulse-sdk/porting/espressif/esp-dsp/modules/common/include"
        "edge-impulse-sdk/porting/espressif/esp-dsp/modules/fft/include"
    REQUIRES esp_timer esp-dsp
)

# Apply optimization (Required for Edge Impulse to run fast)
target_compile_options(${COMPONENT_LIB} PRIVATE -O3)

# Turn off strict warning-as-error behavior
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=missing-field-initializers)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=deprecated-declarations)

# Turn off strict warning-as-error behavior
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=missing-field-initializers)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=deprecated-declarations)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-error=maybe-uninitialized")
