#version 330 core
out vec4 ColorFragment;

in vec3 Normal;
in vec3 PositionFragment;
in float Tolerance;

uniform vec3 colorLight;
uniform vec3 colorNormal;
uniform vec3 colorGouge;
uniform vec3 colorExcess;
uniform vec3 positionView;
uniform vec3 positionLight;
uniform float toleranceGouge;
uniform float toleranceExcess;

float sigmoid(float x)
{
    return (1 / (1 + exp(-x)));
}

vec3 hsv2rgb(vec3 c) {
    vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
    rgb = rgb * rgb * (3.0 - 2.0 * rgb);
    return c.z * mix(vec3(1.0), rgb, c.y);
}


vec3 mapDistanceToColor(float distance, float a, float b) {
    float hue;
    if (distance < a) {
        hue = 0.0;
    } else if (distance > b) {
        hue = 0.6667;
    } else if (distance < 0.0) {
        hue = mix(0.0, 0.3333, (distance - a) / (0.0 - a));
    } else {
        hue = mix(0.3333, 0.6667, distance / b);
    }
    return hsv2rgb(vec3(hue, 1.0, 1.0));
}



vec3 mapDistanceToColor2(float distance, float a, float b) {
    float hue;
    if (distance < 4 * a) {
        hue = 0.0;
    } else if (distance > 4 * b) {
        hue = 0.6667;
    }else if(distance > a && distance < b){
        hue = 0.3333;
    }
    else if (distance < 0.0) {
        hue = mix(0.0, 0.3333, (distance - a - 3 * a) / (0.0 - 3 * a));
    } else {
        hue = mix(0.3333, 0.6667, (distance -  b) / 3  * b);
    }
    return hsv2rgb(vec3(hue, 1.0, 1.0));
}






vec3 mapDistanceToRGB(float D) {
    vec3 color;
    
    // if (D < 0.0) {
    //     // D小于0时的映射
    //     //float normalizedD = clamp(D / 100.0, -1.0, 0.0); // 将D归一化到[-1, 0]范围
    //     float normalizedD = clamp(D, -1.0, 0.0); // 将D归一化到[-1, 0]范围
    //     color.r = (1.0 + normalizedD) * 255.0; // 红色通道R插值
    //     color.g = 255.0; // 绿色通道G取最大值
    //     color.b = (1.0 - normalizedD) * 255.0; // 蓝色通道B插值
    // } else {
    //     // D大于等于0时的映射
    //     //float normalizedD = clamp(D / 100.0, 0.0, 1.0); 
    //     float normalizedD = clamp(D, 0.0, 1.0); // 将D归一化到[0, 1]范围
    //     color.r = 255.0; // 红色通道R取最大值
    //     color.g = (1.0 - normalizedD) * 255.0; // 绿色通道G插值
    //     color.b = 0.0; // 蓝色通道B取最小值
    // }

    if (D < 0.0) {
        //D小于0,过切,应该显示红色,D越小,红色越浓,越接近0,绿色越浓
        // D小于0时的映射
        //float normalizedD = clamp(D / 100.0, -1.0, 0.0); // 将D归一化到[-1, 0]范围
        float normalizedD = clamp(D  + toleranceExcess, -1.0, 0.0); // 将D归一化到[-1, 0]范围
        color.r = abs(normalizedD)  * 255.0; // 红色通道R插值
        color.g = (1.0 + normalizedD) * 255.0; // 绿色通道G取最大值
        color.b = 0.0f; // 蓝色通道B插值
    } else {
        //Dd大于0,残留,应该显示蓝色,D越大,蓝色越浓,越接近0,绿色越浓
        // D大于等于0时的映射
        //float normalizedD = clamp(D / 100.0, 0.0, 1.0); 
        float normalizedD = clamp(D  - toleranceGouge, 0.0, 1.0); // 将D归一化到[0, 1]范围
        color.r = 0.0; // 红色通道R取最大值
        color.g = (1.0 - normalizedD) * 255.0; // 绿色通道G插值
        color.b = normalizedD * 255.0; // 蓝色通道B取最小值
    }
    
    // if(D < 0.0){
    //    float normalizedD  = 1.0;
    //    if(abs(D) < toleranceExcess){
    //         normalizedD = (toleranceExcess - abs(D)) / toleranceExcess;
    //    }
    //     // float normalizedD = clamp(D  + toleranceExcess, -1.0, 0.0); // 将D归一化到[-1, 0]范围
    //     color.r = abs(normalizedD)  * 255.0; // 红色通道R插值
    //     color.g = (1.0 - normalizedD) * 255.0; // 绿色通道G取最大值
    //     color.b = 0.0f; // 蓝色通道B插值
    // }else{
    //    float normalizedD  = 1.0;
    //    if(D < toleranceGouge){
    //         normalizedD = (toleranceGouge - abs(D)) / toleranceGouge;
    //    }
    //     // float normalizedD = clamp(D  + toleranceExcess, -1.0, 0.0); // 将D归一化到[-1, 0]范围
    //     color.r = 0.0; // 红色通道R插值
    //     color.g = (1.0 - normalizedD) * 255.0; // 绿色通道G取最大值
    //     color.b = abs(normalizedD)  * 255.0; // 蓝色通道B插值
    // }

    return color / 255.0; // 将颜色值归一化到[0, 1]范围
}

void main()
{
    vec3 colorObject = colorNormal;
    // if (Tolerance > 0 && Tolerance > toleranceGouge) {
    //     colorObject = colorGouge;
    // } else if (Tolerance < 0 && -Tolerance > toleranceExcess) {
    //     colorObject = colorExcess;
    // }

// if((Tolerance > 0 && Tolerance < toleranceGouge) || (Tolerance < 0 && -Tolerance < toleranceExcess)  ){
//     colorObject.r = 0;
//     colorObject.g = 255;
//     colorObject.b = 0;
//     colorObject = colorObject / 255.0;

// }else{
//     float a = ((sigmoid(Tolerance) + 1.0f)/ 2.0f);
//      a = sigmoid(Tolerance);
//     // colorObject.x = a ;
//     // colorObject.z = a ;
//     // colorObject  = normalize(colorObject);

//     colorObject = mapDistanceToRGB(Tolerance);
// }

colorObject = mapDistanceToColor2(Tolerance,-toleranceExcess,toleranceGouge);





    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * colorLight;

    vec3 directionLight = normalize(positionLight - PositionFragment);
    vec3 diffuse = colorLight * max(dot(Normal, directionLight), 0.0);

    vec3 directionView = normalize(positionView - PositionFragment);
    vec3 directionReflect = reflect(-directionLight, Normal);
    vec3 directionHalfway = normalize(directionLight + directionView);
    vec3 specular = colorLight * pow(max(dot(Normal, directionHalfway), 0.0), 32.0);

    vec3 result = (ambient + diffuse + specular) * colorObject;
    ColorFragment = vec4(result, 1.0);
}
