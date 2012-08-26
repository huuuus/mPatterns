#ifndef mPatternsMac_fboTools_h
#define mPatternsMac_fboTools_h

void displayFbo(gl::Fbo &fbo, Color c = Color::white()) 
{
    gl::color( c );
    gl::draw(fbo.getTexture());            
}

void getFboData_RGBA(gl::Fbo &fbo, void* destRGBA) {
    gl::Texture texFbo = fbo.getTexture();
    texFbo.bind();
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    glGetTexImage( texFbo.getTarget(), 0, GL_RGBA, GL_UNSIGNED_BYTE, destRGBA );
    GLenum err = glGetError();
    if (err!=GL_NO_ERROR)
        printf("err=%x\n",err);
    texFbo.unbind();
}

void createFbo(int w, int h, GLenum glFormat, int nbSamples) {
    gl::Fbo::Format format;
    format.setSamples( 4 );
    format.setColorInternalFormat( GL_RGBA8 );
    mFbo = gl::Fbo(_SCREEN_SIZE, format );
}

void bindAndSetViewport(gl::Fbo &fbo) {
    fbo.bindFramebuffer();    
    gl::setViewport( fbo.getBounds() );
}

#endif
