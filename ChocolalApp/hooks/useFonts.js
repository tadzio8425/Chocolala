import * as Font from 'expo-font';

//Cargar los Fonts
export default useFonts  = async () => 
    await Font.loadAsync({
        'Source-Sans-Black': require('../assets/fonts/SourceSans/SourceSansPro-Black.ttf'),
        'Source-Sans-SemiBold': require('../assets/fonts/SourceSans/SourceSansPro-SemiBold.ttf'),
        'Source-Sans-Regular': require('../assets/fonts/SourceSans/SourceSansPro-Regular.ttf')
  });
