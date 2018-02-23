//
//  VROTypefaceiOS.hpp
//  ViroRenderer
//
//  Created by Raj Advani on 11/24/16.
//  Copyright © 2016 Viro Media. All rights reserved.
//

#ifndef VROTypefaceiOS_h
#define VROTypefaceiOS_h

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#include "VROTypeface.h"
#include <string>

class VRODriver;

class VROTypefaceiOS : public VROTypeface {
    
public:
    
    VROTypefaceiOS(std::string name, int size, std::shared_ptr<VRODriver> driver);
    virtual ~VROTypefaceiOS();
    
    float getLineHeight();
    std::shared_ptr<VROGlyph> loadGlyph(FT_ULong charCode, bool forRendering);
    
protected:
    
    FT_Face loadFace(std::string name, int size, FT_Library ft);
    
private:

    std::weak_ptr<VRODriver> _driver;
    
    // Font data must not be deallocated until the typeface is destroyed
    NSData *_fontData;
    NSData *getFontData(CGFontRef cgFont);
  
};

#endif /* VROTypefaceiOS_h */