// Copyright 2018 bianchui. All rights reserved.
#import "GameViewController.h"
#import "Renderer.h"
#include "inc_xxprofile.hpp"

@implementation GameViewController {
    MTKView *_view;

    id<MTLDevice> _device;

    Renderer *_renderer;
}

- (void)viewDidLoad {
    XX_PROFILE_SCOPE_FUNCTION();
    [super viewDidLoad];

    // Set the view to use the default device
    _device = MTLCreateSystemDefaultDevice();
    _view = (MTKView *)self.view;
    _view.delegate = self;
    _view.device = _device;

    _renderer = [[Renderer alloc] initWithMetalDevice:_device renderDestinationProvider:self];

    [_renderer drawRectResized:_view.bounds.size];

    if(!_device) {
        NSLog(@"Metal is not supported on this device");
        self.view = [[UIView alloc] initWithFrame:self.view.frame];
    }
} 

// Called whenever view changes orientation or layout is changed
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
    XX_PROFILE_SCOPE_FUNCTION();
    [_renderer drawRectResized:view.bounds.size];
}

// Called whenever the view needs to render
- (void)drawInMTKView:(nonnull MTKView *)view {
    XX_PROFILE_SCOPE_FUNCTION();
    @autoreleasepool {
        [_renderer update];
    }
}

// Methods to get and set state of the our ultimate render destination (i.e. the drawable)
# pragma mark RenderDestinationProvider implementation

- (MTLRenderPassDescriptor*) currentRenderPassDescriptor {
    XX_PROFILE_SCOPE_FUNCTION();
    return _view.currentRenderPassDescriptor;
}

- (MTLPixelFormat) colorPixelFormat {
    XX_PROFILE_SCOPE_FUNCTION();
    return _view.colorPixelFormat;
}

- (void) setColorPixelFormat: (MTLPixelFormat) pixelFormat {
    XX_PROFILE_SCOPE_FUNCTION();
    _view.colorPixelFormat = pixelFormat;
}

- (MTLPixelFormat) depthStencilPixelFormat {
    XX_PROFILE_SCOPE_FUNCTION();
    return _view.depthStencilPixelFormat;
}

- (void) setDepthStencilPixelFormat: (MTLPixelFormat) pixelFormat {
    XX_PROFILE_SCOPE_FUNCTION();
    _view.depthStencilPixelFormat = pixelFormat;
}

- (NSUInteger) sampleCount {
    XX_PROFILE_SCOPE_FUNCTION();
    return _view.sampleCount;
}

- (void) setSampleCount:(NSUInteger) sampleCount {
    XX_PROFILE_SCOPE_FUNCTION();
    _view.sampleCount = sampleCount;
}

- (id<MTLDrawable>) currentDrawable {
    XX_PROFILE_SCOPE_FUNCTION();
    return _view.currentDrawable;
}

@end

