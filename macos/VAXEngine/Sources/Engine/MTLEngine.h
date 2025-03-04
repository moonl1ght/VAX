//
// Created by Alexander Lakhonin on 10.02.2025.
//

#import <Foundation/Foundation.h>
#import <MetalKit/MetalKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface MTLEngine: NSObject

- (nonnull CAMetalLayer *)getRenderingLayer;
- (void) load;
- (void) resize: (CGSize) viewSize drawableSize: (CGSize) drawableSize;
- (void) configure: (MTKView *)view;
- (void) drawIn: (MTKView *)view;

@end

NS_ASSUME_NONNULL_END
