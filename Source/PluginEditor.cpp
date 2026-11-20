#include "PluginEditor.h"
#include <cmath>

namespace Colors
{
    const juce::Colour bg        (0xff0d0b14);
    const juce::Colour panelTop  (0xff1b1628);
    const juce::Colour panelBot  (0xff100d19);
    const juce::Colour lcdBg     (0xff080513);
    const juce::Colour border    (0xff382759);
    const juce::Colour purple    (0xff8b3dff);
    const juce::Colour neon      (0xffc77dff);
    const juce::Colour glow      (0xffedd4ff);
    const juce::Colour hotPink   (0xffff3e8e);
    const juce::Colour text      (0xffe9e2f5);
    const juce::Colour subText   (0xff9889b5);
}

static juce::String u8(const char* s) { return juce::String::fromUTF8(s); }

static void disableFocusRecursive(juce::Component& comp)
{
    comp.setWantsKeyboardFocus(false);
    comp.setMouseClickGrabsKeyboardFocus(false);
    for (auto* child : comp.getChildren())
        disableFocusRecursive(*child);
}

WitchLookAndFeel::WitchLookAndFeel()
{
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff130f1f));
    setColour(juce::ComboBox::outlineColourId, Colors::border);
    setColour(juce::ComboBox::textColourId, Colors::text);
    setColour(juce::ComboBox::arrowColourId, Colors::neon);
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff120e1e));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, Colors::purple);
    setColour(juce::PopupMenu::textColourId, Colors::text);
    setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1b152a));
    setColour(juce::TextButton::buttonOnColourId, Colors::purple);
    setColour(juce::TextButton::textColourOffId, Colors::glow);
    setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    setColour(juce::ToggleButton::textColourId, Colors::text);
    setColour(juce::Slider::textBoxTextColourId, Colors::glow);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff090611));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::ListBox::outlineColourId, juce::Colours::transparentBlack);
    setColour(juce::ScrollBar::thumbColourId, Colors::purple.withAlpha(0.75f));
}

void WitchLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
                                        float pos, float startAngle, float endAngle, juce::Slider& s)
{
    const float radius = static_cast<float>(juce::jmin(w, h)) * 0.40f;
    const float cx = static_cast<float>(x) + static_cast<float>(w) * 0.5f;
    const float cy = static_cast<float>(y) + static_cast<float>(h) * 0.48f;
    const float angle = startAngle + pos * (endAngle - startAngle);

    // MIX/MASTER = красный акцент, остальное неоново-фиолетовое
    const auto name = s.getName();
    const bool hot = name.containsIgnoreCase("MIX") || name.containsIgnoreCase("MASTER");
    const juce::Colour arcCol = hot ? juce::Colour(0xffff1144) : Colors::neon;
    const juce::Colour arcGlow = hot ? juce::Colour(0xffff3355) : Colors::purple;

    const float tickR = radius + 4.0f;
    for (int i = 0; i <= 10; ++i)
    {
        const float tNorm = static_cast<float>(i) / 10.0f;
        const float tAng = startAngle + tNorm * (endAngle - startAngle);
        const float tx = cx + std::sin(tAng) * tickR;
        const float ty = cy - std::cos(tAng) * tickR;
        g.setColour(tNorm <= pos + 0.02f ? arcCol : juce::Colour(0xff382b52));
        g.fillEllipse(tx - 1.2f, ty - 1.2f, 2.4f, 2.4f);
    }

    juce::Path track;
    track.addCentredArc(cx, cy, radius, radius, 0.0f, startAngle, endAngle, true);
    g.setColour(juce::Colour(0xff0b0813));
    g.strokePath(track, juce::PathStrokeType(3.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path glowArc;
    glowArc.addCentredArc(cx, cy, radius, radius, 0.0f, startAngle, angle, true);
    g.setColour(arcGlow.withAlpha(0.40f));
    g.strokePath(glowArc, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour(arcCol);
    g.strokePath(glowArc, juce::PathStrokeType(2.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const float innerR = radius * 0.74f;
    g.setColour(juce::Colour(0xaa000000));
    g.fillEllipse(cx - innerR + 1.5f, cy - innerR + 2.5f, innerR * 2.0f, innerR * 2.0f);

    juce::ColourGradient metalGrad(juce::Colour(0xff3a2f52), cx - innerR * 0.6f, cy - innerR * 0.7f,
                                   juce::Colour(0xff110d1b), cx + innerR * 0.7f, cy + innerR * 0.8f, true);
    g.setGradientFill(metalGrad);
    g.fillEllipse(cx - innerR, cy - innerR, innerR * 2.0f, innerR * 2.0f);

    g.setColour(juce::Colour(0x45ffffff));
    g.drawEllipse(cx - innerR + 0.5f, cy - innerR + 0.5f, innerR * 2.0f - 1.0f, innerR * 2.0f - 1.0f, 1.0f);

    const float coreR = innerR * 0.74f;
    g.setColour(juce::Colour(0xff1b1529));
    g.fillEllipse(cx - coreR, cy - coreR, coreR * 2.0f, coreR * 2.0f);

    juce::Path ptr;
    ptr.addRoundedRectangle(-1.3f, -innerR + 2.0f, 2.6f, innerR * 0.55f, 1.0f);
    ptr.applyTransform(juce::AffineTransform::rotation(angle).translated(cx, cy));
    g.setColour(hot ? juce::Colour(0xffff6688) : Colors::glow);
    g.fillPath(ptr);
}

void WitchLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& b, bool highlighted, bool down)
{
    const auto r = b.getLocalBounds().toFloat().reduced(1.0f);
    const bool on = b.getToggleState();

    juce::ColourGradient keyGrad(on ? juce::Colour(0xff342254) : juce::Colour(0xff201930), 0.0f, r.getY(),
                                 on ? juce::Colour(0xff1d1330) : juce::Colour(0xff110d1a), 0.0f, r.getBottom(), false);
    if (down) keyGrad = juce::ColourGradient(juce::Colour(0xff120d1c), 0.0f, r.getY(), juce::Colour(0xff201833), 0.0f, r.getBottom(), false);
    g.setGradientFill(keyGrad);
    g.fillRoundedRectangle(r, 4.0f);

    g.setColour(on ? Colors::neon.withAlpha(0.85f) : (highlighted ? Colors::purple : Colors::border));
    g.drawRoundedRectangle(r, 4.0f, 1.1f);

    const float ledR = 3.5f;
    const float lx = r.getX() + 9.0f;
    const float ly = r.getCentreY();
    if (on)
    {
        g.setColour(Colors::neon.withAlpha(0.35f));
        g.fillEllipse(lx - 6.5f, ly - 6.5f, 13.0f, 13.0f);
        g.setColour((b.getButtonText() == "T.STOP" || b.getButtonText() == "FREEZE") ? Colors::hotPink : Colors::glow);
        g.fillEllipse(lx - ledR, ly - ledR, ledR * 2.0f, ledR * 2.0f);
    }
    else
    {
        g.setColour(juce::Colour(0xff2a203d));
        g.fillEllipse(lx - ledR, ly - ledR, ledR * 2.0f, ledR * 2.0f);
    }

    g.setColour(on ? juce::Colours::white : Colors::subText);
    g.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    g.drawText(b.getButtonText(), static_cast<int>(r.getX()) + 16, static_cast<int>(r.getY()),
               static_cast<int>(r.getWidth()) - 18, static_cast<int>(r.getHeight()), juce::Justification::centred, true);
}

void WitchLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& b, const juce::Colour& bgCol,
                                            bool highlighted, bool down)
{
    const auto r = b.getLocalBounds().toFloat().reduced(0.5f);
    const auto topC = down ? bgCol.darker(0.3f) : (highlighted ? bgCol.brighter(0.18f) : bgCol.brighter(0.08f));
    const auto botC = down ? bgCol.brighter(0.1f) : bgCol.darker(0.35f);
    g.setGradientFill(juce::ColourGradient(topC, 0.0f, r.getY(), botC, 0.0f, r.getBottom(), false));
    g.fillRoundedRectangle(r, 4.0f);

    g.setColour(juce::Colour(0x28ffffff));
    g.drawHorizontalLine(static_cast<int>(r.getY()) + 1, r.getX() + 2.0f, r.getRight() - 2.0f);
    g.setColour(b.getToggleState() ? Colors::glow : Colors::border);
    g.drawRoundedRectangle(r, 4.0f, 1.0f);
}

void WitchLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool,
                                    int buttonX, int, int buttonW, int, juce::ComboBox&)
{
    const juce::Rectangle<float> r(0.5f, 0.5f, static_cast<float>(width) - 1.0f, static_cast<float>(height) - 1.0f);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff0b0813), 0.0f, 0.0f, juce::Colour(0xff171126), 0.0f, static_cast<float>(height), false));
    g.fillRoundedRectangle(r, 3.5f);
    g.setColour(Colors::border);
    g.drawRoundedRectangle(r, 3.5f, 1.0f);

    const float cx = static_cast<float>(buttonX) + static_cast<float>(buttonW) * 0.5f;
    const float cy = static_cast<float>(height) * 0.5f;
    juce::Path arrow;
    arrow.addTriangle(cx - 4.0f, cy - 2.0f, cx + 4.0f, cy - 2.0f, cx, cy + 3.0f);
    g.setColour(Colors::neon);
    g.fillPath(arrow);
}

void WitchListModel::paintListBoxItem(int row, juce::Graphics& g, int w, int h, bool selected)
{
    if (selected)
    {
        g.setGradientFill(juce::ColourGradient(Colors::purple.withAlpha(0.90f), 0.0f, 0.0f,
                                               juce::Colour(0xff5e23a6).withAlpha(0.90f), static_cast<float>(w), 0.0f, false));
        g.fillRoundedRectangle(3.0f, 1.0f, static_cast<float>(w) - 6.0f, static_cast<float>(h) - 2.0f, 3.0f);
        g.setColour(Colors::glow.withAlpha(0.5f));
        g.drawRoundedRectangle(3.0f, 1.0f, static_cast<float>(w) - 6.0f, static_cast<float>(h) - 2.0f, 3.0f, 0.8f);
    }
    else if (row % 2 == 1)
    {
        g.setColour(juce::Colour(0x0cffffff));
        g.fillRect(2, 0, w - 4, h);
    }

    g.setColour(selected ? juce::Colours::white : Colors::text.withAlpha(0.92f));
    g.setFont(juce::FontOptions(12.0f, selected ? juce::Font::bold : juce::Font::plain));
    if (row >= 0 && row < static_cast<int>(items.size()))
        g.drawText(items[static_cast<size_t>(row)], 8, 0, w - 12, h, juce::Justification::left, true);
}

WitchXYPad::WitchXYPad(juce::AudioProcessorValueTreeState& state) : apvts(state) {}

void WitchXYPad::paint(juce::Graphics& g)
{
    const auto b = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xff080512));
    g.fillRoundedRectangle(b, 6.0f);
    g.setColour(Colors::border);
    g.drawRoundedRectangle(b, 6.0f, 1.2f);

    g.setColour(juce::Colour(0xff23183b));
    for (int i = 1; i < 4; ++i)
    {
        const float gx = b.getX() + b.getWidth() * (static_cast<float>(i) * 0.25f);
        const float gy = b.getY() + b.getHeight() * (static_cast<float>(i) * 0.25f);
        g.drawVerticalLine(static_cast<int>(gx), b.getY() + 2.0f, b.getBottom() - 2.0f);
        g.drawHorizontalLine(static_cast<int>(gy), b.getX() + 2.0f, b.getRight() - 2.0f);
    }

    const float mx = apvts.getRawParameterValue("macroX")->load();
    const float my = apvts.getRawParameterValue("macroY")->load();
    const float px = b.getX() + 8.0f + mx * (b.getWidth() - 16.0f);
    const float py = b.getBottom() - 8.0f - my * (b.getHeight() - 16.0f);

    g.setColour(Colors::purple.withAlpha(0.35f));
    g.drawHorizontalLine(static_cast<int>(py), b.getX() + 2.0f, b.getRight() - 2.0f);
    g.drawVerticalLine(static_cast<int>(px), b.getY() + 2.0f, b.getBottom() - 2.0f);

    g.setColour(Colors::purple.withAlpha(0.30f));
    g.fillEllipse(px - 18.0f, py - 18.0f, 36.0f, 36.0f);
    g.setColour(Colors::neon);
    g.fillEllipse(px - 6.0f, py - 6.0f, 12.0f, 12.0f);
    g.setColour(Colors::glow);
    g.drawEllipse(px - 8.0f, py - 8.0f, 16.0f, 16.0f, 1.5f);

    g.setColour(Colors::subText);
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.drawText("DIMENSION", static_cast<int>(b.getX()) + 8, static_cast<int>(b.getY()) + 6, 80, 14, juce::Justification::left);
    g.drawText("CHAOS", static_cast<int>(b.getRight()) - 72, static_cast<int>(b.getBottom()) - 18, 64, 14, juce::Justification::right);
}

void WitchXYPad::mouseDown(const juce::MouseEvent& e) { updateFromMouse(e); }
void WitchXYPad::mouseDrag(const juce::MouseEvent& e) { updateFromMouse(e); }

void WitchXYPad::updateFromMouse(const juce::MouseEvent& e)
{
    const float nx = juce::jlimit(0.0f, 1.0f, (static_cast<float>(e.x) - 8.0f) / juce::jmax(1.0f, static_cast<float>(getWidth() - 16)));
    const float ny = juce::jlimit(0.0f, 1.0f, 1.0f - (static_cast<float>(e.y) - 8.0f) / juce::jmax(1.0f, static_cast<float>(getHeight() - 16)));
    if (auto* px = apvts.getParameter("macroX")) px->setValueNotifyingHost(nx);
    if (auto* py = apvts.getParameter("macroY")) py->setValueNotifyingHost(ny);
    repaint();
}

WitchStepMatrix::WitchStepMatrix(juce::AudioProcessorValueTreeState& state, std::atomic<int>& stepRef)
    : apvts(state), activeStep(stepRef) {}

void WitchStepMatrix::paint(juce::Graphics& g)
{
    const float w = static_cast<float>(getWidth()) / 8.0f;
    const float h = static_cast<float>(getHeight());
    const int arpMode = static_cast<int>(apvts.getRawParameterValue("arpMode")->load());
    const int cur = activeStep.load(std::memory_order_relaxed);
    static constexpr const char* lbls[4] = { "--", "ON", "+12", "2X" };

    for (int i = 0; i < 8; ++i)
    {
        const int val = static_cast<int>(apvts.getRawParameterValue("seq" + juce::String(i))->load());
        const juce::Rectangle<float> r(static_cast<float>(i) * w + 2.0f, 2.0f, w - 4.0f, h - 4.0f);

        juce::Colour c(0xff191328);
        if (val == 1) c = Colors::purple;
        else if (val == 2) c = Colors::neon;
        else if (val == 3) c = Colors::hotPink;

        const bool isPlayhead = (arpMode > 0) && (i == cur);
        g.setColour(isPlayhead ? c.brighter(0.35f) : c.withAlpha(val == 0 ? 0.90f : 0.78f));
        g.fillRoundedRectangle(r, 4.0f);
        g.setColour(isPlayhead ? juce::Colours::white : Colors::border);
        g.drawRoundedRectangle(r, 4.0f, isPlayhead ? 1.6f : 1.0f);

        g.setColour(val == 0 ? Colors::subText : juce::Colours::white);
        g.setFont(juce::FontOptions(10.5f, juce::Font::bold));
        g.drawText(lbls[juce::jlimit(0, 3, val)], r, juce::Justification::centred);
    }
}

void WitchStepMatrix::mouseDown(const juce::MouseEvent& e)
{
    const int idx = juce::jlimit(0, 7, static_cast<int>(static_cast<float>(e.x) / (static_cast<float>(getWidth()) / 8.0f)));
    if (auto* p = apvts.getParameter("seq" + juce::String(idx)))
    {
        const int cur = static_cast<int>(apvts.getRawParameterValue("seq" + juce::String(idx))->load());
        const int next = (cur + 1) % 4;
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(next)));
        repaint();
    }
}

WitchSynthAudioProcessorEditor::WitchSynthAudioProcessorEditor(WitchSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), xyPad(p.apvts), stepMatrix(p.apvts, p.arpStepDisplay),
      midiKeyboard(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel(&lnf);
    activeHelpText = u8("РќР°РІРµРґРёС‚Рµ РєСѓСЂСЃРѕСЂ РЅР° Р»СЋР±СѓСЋ СЂСѓС‡РєСѓ, РєР°С‚РµРіРѕСЂРёСЋ РёР»Рё РєРЅРѕРїРєСѓ вЂ” Р·РґРµСЃСЊ РїРѕСЏРІРёС‚СЃСЏ РѕРїРёСЃР°РЅРёРµ РЅР° СЂСѓСЃСЃРєРѕРј СЏР·С‹РєРµ.");

    categoryModel.items.push_back(u8("★ Favorites"));
    const auto& folders = getFactoryFolders();
    for (int i = 0; i < static_cast<int>(folders.size()); ++i)
        categoryModel.items.push_back(juce::String(i + 1).paddedLeft('0', 2) + ". " + folders[static_cast<size_t>(i)]);

    categoryModel.onRowClick = [this](int row)
    {
        if (row >= 0 && row < static_cast<int>(categoryModel.items.size()))
        {
            selectedUiCategory = row;
            rebuildPresetListForCategory(selectedUiCategory);
            if (!visiblePresetIndices.empty())
            {
                presetList.selectRow(0, false, true);
                audioProcessor.loadPreset(visiblePresetIndices[0]);
                lastSeenPreset = audioProcessor.getCurrentPresetIndex();
                updateLikeButtonVisual();
                repaint(330, 12, 500, 382);
            }
        }
        giveAwayKeyboardFocus();
    };

    presetModel.onRowClick = [this](int row)
    {
        if (row >= 0 && row < static_cast<int>(visiblePresetIndices.size()))
        {
            audioProcessor.loadPreset(visiblePresetIndices[static_cast<size_t>(row)]);
            lastSeenPreset = audioProcessor.getCurrentPresetIndex();
            updateLikeButtonVisual();
            repaint(330, 12, 500, 382);
        }
        giveAwayKeyboardFocus();
    };

    categoryList.setRowHeight(22);
    presetList.setRowHeight(21);
    categoryList.setOutlineThickness(0);
    presetList.setOutlineThickness(0);
    addAndMakeVisible(categoryList);
    addAndMakeVisible(presetList);

    prevBtn.onClick = [this]
    {
        const int total = static_cast<int>(getAllPresets().size());
        audioProcessor.loadPreset((audioProcessor.getCurrentPresetIndex() - 1 + total) % total);
        syncPresetUI();
        giveAwayKeyboardFocus();
    };
    nextBtn.onClick = [this]
    {
        const int total = static_cast<int>(getAllPresets().size());
        audioProcessor.loadPreset((audioProcessor.getCurrentPresetIndex() + 1) % total);
        syncPresetUI();
        giveAwayKeyboardFocus();
    };
    likeBtn.onClick = [this]
    {
        audioProcessor.toggleFavorite(audioProcessor.getCurrentPresetIndex());
        updateLikeButtonVisual();
        rebuildPresetListForCategory(selectedUiCategory);
        syncPresetUI();
        giveAwayKeyboardFocus();
    };
    rndBtn.onClick = [this] { randomizeWitchPreset(); giveAwayKeyboardFocus(); };
    initBtn.onClick = [this] { audioProcessor.initPatch(); giveAwayKeyboardFocus(); };
    panicBtn.onClick = [this] { audioProcessor.triggerPanic(); giveAwayKeyboardFocus(); };

    kbdBtn.setClickingTogglesState(true);
    kbdBtn.setToggleState(showKeyboard, juce::dontSendNotification);
    kbdBtn.onClick = [this]
    {
        showKeyboard = kbdBtn.getToggleState();
        midiKeyboard.setVisible(showKeyboard);
        setSize(1160, showKeyboard ? 790 : 720);
        giveAwayKeyboardFocus();
    };

    lockFxBtn.setClickingTogglesState(true);
    lockFxBtn.setToggleState(audioProcessor.lockFx.load(), juce::dontSendNotification);
    lockFxBtn.onClick = [this] { audioProcessor.lockFx.store(lockFxBtn.getToggleState()); giveAwayKeyboardFocus(); };

    pageMainBtn.onClick = [this] { setSidePage(SidePage::Main); giveAwayKeyboardFocus(); };
    pageOscBtn.onClick  = [this] { setSidePage(SidePage::Osc); giveAwayKeyboardFocus(); };
    pageArpBtn.onClick  = [this] { setSidePage(SidePage::Arp); giveAwayKeyboardFocus(); };
    pageXyBtn.onClick   = [this] { setSidePage(SidePage::XY); giveAwayKeyboardFocus(); };

    addAndMakeVisible(prevBtn); addAndMakeVisible(nextBtn); addAndMakeVisible(likeBtn);
    addAndMakeVisible(rndBtn);  addAndMakeVisible(initBtn); addAndMakeVisible(kbdBtn);
    addAndMakeVisible(lockFxBtn); addAndMakeVisible(panicBtn);
    addAndMakeVisible(pageMainBtn); addAndMakeVisible(pageOscBtn); addAndMakeVisible(pageArpBtn); addAndMakeVisible(pageXyBtn);

    midiKeyboard.setAvailableRange(36, 96);
    midiKeyboard.setKeyWidth(19.0f);
    midiKeyboard.setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour(0xffd7ceea));
    midiKeyboard.setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xff130e1e));
    midiKeyboard.setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, Colors::border);
    midiKeyboard.setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, Colors::purple.withAlpha(0.45f));
    midiKeyboard.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, Colors::neon);
    addAndMakeVisible(midiKeyboard);

    registerHelp(&categoryList, u8("РљР°С‚РµРіРѕСЂРёРё Р·РІСѓРєРѕРІ: РІС‹Р±РµСЂРёС‚Рµ С‚РёРї (Bass, Lead, Pad, Pluck, Arp, Choir, FX) РёР»Рё РР·Р±СЂР°РЅРЅРѕРµ (★ Favorites)."));
    registerHelp(&presetList, u8("РЎРїРёСЃРѕРє Р·РІСѓРєРѕРІ РїРѕРґ С„РёРѕР»РµС‚РѕРІС‹Рј СЃС‚РµРєР»РѕРј: РєР»РёРєРЅРёС‚Рµ РїРѕ РїСЂРµСЃРµС‚Сѓ, С‡С‚РѕР±С‹ РјРіРЅРѕРІРµРЅРЅРѕ Р·Р°РіСЂСѓР·РёС‚СЊ РµРіРѕ."));
    registerHelp(&prevBtn, u8("РџСЂРµРґС‹РґСѓС‰РёР№ РїСЂРµСЃРµС‚."));
    registerHelp(&nextBtn, u8("РЎР»РµРґСѓСЋС‰РёР№ РїСЂРµСЃРµС‚."));
    registerHelp(&likeBtn, u8("Р›Р°Р№Рє (♥): РґРѕР±Р°РІР»СЏРµС‚ РёР»Рё СѓРґР°Р»СЏРµС‚ С‚РµРєСѓС‰РёР№ Р·РІСѓРє РІ РїР°РїРєСѓ ★ Favorites (СЃРѕС…СЂР°РЅСЏРµС‚СЃСЏ РЅР° РґРёСЃРє)."));
    registerHelp(&rndBtn, u8("RND: СЃР»СѓС‡Р°Р№РЅР°СЏ РіРµРЅРµСЂР°С†РёСЏ РјСЂР°С‡РЅРѕРіРѕ Witch House Р·РІСѓРєР° Рё СЂРёС‚РјР° СЃРµРєРІРµРЅСЃРѕСЂР°."));
    registerHelp(&initBtn, u8("INIT: СЃР±СЂРѕСЃ РІСЃРµС… СЂСѓС‡РµРє РІ С‡РёСЃС‚С‹Р№ Р·РІСѓРє РґР»СЏ СЃРѕР·РґР°РЅРёСЏ РїР°С‚С‡Р° СЃ РЅСѓР»СЏ."));
    registerHelp(&kbdBtn, u8("KBD: РїРѕРєР°Р·С‹РІР°РµС‚ РёР»Рё СѓР±РёСЂР°РµС‚ СЌРєСЂР°РЅРЅСѓСЋ РїРёР°РЅРёРЅРѕ-РєР»Р°РІРёР°С‚СѓСЂСѓ РІРЅРёР·Сѓ РѕРєРЅР°."));
    registerHelp(&lockFxBtn, u8("LOCK FX: СЃРѕС…СЂР°РЅСЏРµС‚ РЅР°СЃС‚СЂРѕР№РєРё СЌС„С„РµРєС‚РѕРІ Рё XY-РїР°РґР° РїСЂРё РїРµСЂРµРєР»СЋС‡РµРЅРёРё Р·РІСѓРєРѕРІ."));
    registerHelp(&panicBtn, u8("PANIC: РјРіРЅРѕРІРµРЅРЅРѕ РіР»СѓС€РёС‚ Р·Р°РІРёСЃС€РёРµ РЅРѕС‚С‹, Freeze Рё Р±РµСЃРєРѕРЅРµС‡РЅС‹Р№ РґРёР»РµР№."));
    registerHelp(&midiKeyboard, u8("Р­РєСЂР°РЅРЅР°СЏ РєР»Р°РІРёР°С‚СѓСЂР°: РЅР°Р¶РёРјР°Р№С‚Рµ РЅР° РєР»Р°РІРёС€Рё РјС‹С€РєРѕР№, РєРЅРѕРїРєР° KBD СЃРІРµСЂС…Сѓ СѓР±РёСЂР°РµС‚ РµС‘."));
    registerHelp(&pageMainBtn, u8("Р’РєР»Р°РґРєР° MAIN: С„РёР»СЊС‚СЂ СЃ Р¶РёРІС‹Рј РіСЂР°С„РёРєРѕРј, С‚РѕРЅР°Р»СЊРЅРѕСЃС‚СЊ, Р¶РёРІРѕР№ РіСЂР°С„РёРє ADSR Рё Morph A↔B."));
    registerHelp(&pageOscBtn, u8("Р’РєР»Р°РґРєР° OSC: РЅР°СЃС‚СЂРѕР№РєР° РІРѕР»РЅ OSC 1 / OSC 2, СЃР°Р±-Р±Р°СЃР° Рё С„РёР·РёРєРё 2040 (Entropy, Gravity, Ghost)."));
    registerHelp(&pageArpBtn, u8("Р’РєР»Р°РґРєР° ARP: LFO-РјРѕРґСѓР»СЏС†РёСЏ, СЃР°Р№РґС‡РµР№РЅ Pump Рё 8-С€Р°РіРѕРІС‹Р№ СЃРµРєРІРµРЅСЃРѕСЂ СЃ РґСЂРѕР±СЏРјРё (Ratchets)."));
    registerHelp(&pageXyBtn, u8("Р’РєР»Р°РґРєР° XY: 2D-РїР°Рґ Aether Core РґР»СЏ РјРіРЅРѕРІРµРЅРЅРѕРіРѕ РјРѕСЂС„РёРЅРіР° Р°РіСЂРµСЃСЃРёРё Рё РїСЂРѕСЃС‚СЂР°РЅСЃС‚РІР°."));

    initCombo(filterTypeBox, "filterType", { "LowPass", "HighPass", "BandPass", "Formant", "Comb", "DualAcid" },
        u8("РўРёРї С„РёР»СЊС‚СЂР°: LP/HP/BP, Formant (РіР»Р°СЃРЅС‹Рµ), Comb (СЃС‚СЂСѓРЅС‹ РїРѕ РЅРѕС‚Р°Рј) Рё DualAcid 24dB."));
    initKnob(filterCutoff, "filterCutoff", "CUTOFF", u8("CUTOFF: С‡Р°СЃС‚РѕС‚Р° СЃСЂРµР·Р° С„РёР»СЊС‚СЂР° (РІ Comb РјРµРЅСЏРµС‚ СЏСЂРєРѕСЃС‚СЊ СЃС‚СЂСѓРЅС‹, РІ Formant вЂ” РіР»Р°СЃРЅСѓСЋ)."));
    initKnob(filterReso, "filterReso", "RESO", u8("RESONANCE: СЂРµР·РѕРЅР°РЅСЃРЅС‹Р№ РїРёРє РЅР° СЃСЂРµР·Рµ (РІ Comb вЂ” РґР»РёС‚РµР»СЊРЅРѕСЃС‚СЊ Р·РІСѓС‡Р°РЅРёСЏ СЃС‚СЂСѓРЅС‹)."));
    initKnob(filterDrive, "filterDrive", "DRIVE", u8("РџРµСЂРµРіСЂСѓР· РЅР° РІС…РѕРґРµ С„РёР»СЊС‚СЂР°."));
    initKnob(filterEnvAmt, "filterEnvAmt", "ENV AMT", u8("Р“Р»СѓР±РёРЅР° РѕРіРёР±Р°СЋС‰РµР№ С„РёР»СЊС‚СЂР° РІ РЅР°С‡Р°Р»Рµ РЅРѕС‚С‹."));
    initKnob(filterDecay, "filterDecay", "F.DECAY", u8("Р’СЂРµРјСЏ Р·Р°РєСЂС‹С‚РёСЏ С„РёР»СЊС‚СЂР° РїРѕСЃР»Рµ РЅР°Р¶Р°С‚РёСЏ РЅРѕС‚С‹."));
    initKnob(filterKeyTrack, "filterKeyTrack", "K.TRK", u8("KEYTRACK: РѕС‚СЃР»РµР¶РёРІР°РЅРёРµ РІС‹СЃРѕС‚С‹ РЅРѕС‚С‹ (РЅР° РІС‹СЃРѕРєРёС… РЅРѕС‚Р°С… С„РёР»СЊС‚СЂ РѕС‚РєСЂС‹РІР°РµС‚СЃСЏ СЏСЂС‡Рµ)."));
    initKnob(tiltAmt, "tiltAmt", "TILT", u8("TILT EQ: Р±Р°Р»Р°РЅСЃ СЃРїРµРєС‚СЂР° (РІР»РµРІРѕ вЂ” С‚РµРјРЅРµРµ/Р¶РёСЂРЅРµРµ, РІРїСЂР°РІРѕ вЂ” СЏСЂС‡Рµ Рё С…РѕР»РѕРґРЅРµРµ)."));
    initKnob(stereoWidth, "stereoWidth", "WIDTH", u8("WIDTH: С€РёСЂРёРЅР° СЃС‚РµСЂРµРѕРїР°РЅРѕСЂР°РјС‹ (РѕС‚ СЃСѓС…РѕРіРѕ РјРѕРЅРѕ РґРѕ С€РёСЂРѕРєРѕРіРѕ РѕР±СЉС‘РјР°)."));

    initCombo(polyModeBox, "polyMode", { "Poly", "Mono" }, u8("Р РµР¶РёРј: Poly (Р°РєРєРѕСЂРґС‹ РґРѕ 8 РЅРѕС‚) РёР»Рё Mono (РѕРґРЅРѕРіРѕР»РѕСЃРЅС‹Р№ Р»РµРіР°С‚Рѕ-Р±Р°СЃ/Р»РёРґ)."));
    const auto& allP = getAllPresets();
    for (int i = 0; i < static_cast<int>(allP.size()); ++i)
        morphPresetBox.addItem("B: " + juce::String(allP[static_cast<size_t>(i)].name), i + 1);
    morphPresetBox.setWantsKeyboardFocus(false);
    morphPresetBox.setMouseClickGrabsKeyboardFocus(false);
    addAndMakeVisible(morphPresetBox);
    comboAtts.push_back(std::make_unique<ComboAtt>(audioProcessor.apvts, "morphPreset", morphPresetBox));
    registerHelp(&morphPresetBox, u8("Р¦РµР»СЊ B РґР»СЏ РјРѕСЂС„РёРЅРіР°: РІС‹Р±РµСЂРёС‚Рµ Р·РІСѓРє, РІ РєРѕС‚РѕСЂС‹Р№ Р±СѓРґРµС‚ РїСЂРµРІСЂР°С‰Р°С‚СЊСЃСЏ С‚РµРєСѓС‰РёР№ РїСЂРµСЃРµС‚."));

    initKnob(ampA, "ampAttack", "ATTACK", u8("ATTACK: РІСЂРµРјСЏ РЅР°СЂР°СЃС‚Р°РЅРёСЏ РіСЂРѕРјРєРѕСЃС‚Рё РЅРѕС‚С‹."));
    initKnob(ampD, "ampDecay", "DECAY", u8("DECAY: РІСЂРµРјСЏ СЃРїР°РґР° РѕС‚ РїРёРєР° Рє СѓСЂРѕРІРЅСЋ Sustain."));
    initKnob(ampS, "ampSustain", "SUSTAIN", u8("SUSTAIN: РіСЂРѕРјРєРѕСЃС‚СЊ, РїРѕРєР° РєР»Р°РІРёС€Р° Р·Р°Р¶Р°С‚Р°."));
    initKnob(ampR, "ampRelease", "RELEASE", u8("RELEASE: РґР»РёРЅР° С…РІРѕСЃС‚Р° РїРѕСЃР»Рµ РѕС‚РїСѓСЃРєР°РЅРёСЏ РєР»Р°РІРёС€Рё."));
    initKnob(glideTime, "glideTime", "GLIDE", u8("GLIDE: РїР»Р°РІРЅРѕРµ СЃРєРѕР»СЊР¶РµРЅРёРµ (Portamento) РѕС‚ РѕРґРЅРѕР№ РЅРѕС‚С‹ Рє РґСЂСѓРіРѕР№."));
    initKnob(snapAmt, "snapAmt", "SNAP", u8("SNAP: СѓСЃРёР»РёС‚РµР»СЊ С‰РµР»С‡РєР° Р°С‚Р°РєРё (РїРµСЂРІС‹Рµ 15 РјСЃ) РґР»СЏ РїСЂРѕР±РёРІРЅС‹С… РїР»Р°РєРѕРІ Рё Р±Р°СЃРѕРІ."));
    initKnob(airAmt, "airAmt", "AIR", u8("CRYSTAL AIR: СЌРєСЃР°Р№С‚РµСЂ РІС‹СЃРѕРєРёС… С‡Р°СЃС‚РѕС‚ (5.5+ kHz) РґР»СЏ СЃС‚РµРєР»СЏРЅРЅРѕРіРѕ РІРѕР·РґСѓС…Р°."));
    initKnob(morphAmt, "morphAmt", "MORPH B", u8("MORPH A↔B: РїР»Р°РІРЅРѕ РїСЂРµРІСЂР°С‰Р°РµС‚ С‚РµРєСѓС‰РёР№ Р·РІСѓРє РІ РїСЂРµСЃРµС‚ B РІ СЂРµР°Р»СЊРЅРѕРј РІСЂРµРјРµРЅРё!"));

    juce::StringArray waves{ "Saw", "Pulse", "Tri", "NES4", "Sine", "Spectral", "ChaosPWM", "AcidForm" };
    initCombo(osc1WaveBox, "osc1Wave", waves, u8("Р’РѕР»РЅР° OSC 1 (Saw, Pulse, Tri, NES4, Sine, Spectral, ChaosPWM, AcidForm)."));
    initCombo(osc2WaveBox, "osc2Wave", waves, u8("Р’РѕР»РЅР° OSC 2 РґР»СЏ РЅР°СЃР»РѕРµРЅРёСЏ Рё FM-РјРѕРґСѓР»СЏС†РёРё."));
    initKnob(osc1Oct, "osc1Oct", "O1 OCT", u8("РћРєС‚Р°РІР° 1-РіРѕ РѕСЃС†РёР»Р»СЏС‚РѕСЂР° (-2..+2)."));
    initKnob(osc1Semi, "osc1Semi", "O1 SEMI", u8("РџРѕР»СѓС‚РѕРЅР° (Pitch) 1-РіРѕ РѕСЃС†РёР»Р»СЏС‚РѕСЂР° (-12..+12)."));
    initKnob(pwmWidth, "pwmWidth", "MORPH", u8("MORPH / PWM: РјРµРЅСЏРµС‚ С€РёСЂРёРЅСѓ РёРјРїСѓР»СЊСЃР° Рё С„РѕСЂРјСѓ РІРѕР»РЅС‹."));
    initKnob(xModAmt, "xModAmt", "FM/SYNC", u8("FM/SYNC: РїРµСЂРµРєСЂС‘СЃС‚РЅР°СЏ РјРѕРґСѓР»СЏС†РёСЏ OSC 1 РѕС‚ OSC 2."));
    initKnob(osc1Detune, "osc1Detune", "DETUNE", u8("DETUNE: СЂР°СЃСЃС‚СЂРѕР№РєР° СЃР»РѕС‘РІ СѓРЅРёСЃРѕРЅР° (Р¶РёСЂРЅС‹Р№ С€РёСЂРѕРєРёР№ Р·РІСѓРє)."));
    initKnob(osc1Level, "osc1Level", "O1 LVL", u8("Р“СЂРѕРјРєРѕСЃС‚СЊ 1-РіРѕ РѕСЃС†РёР»Р»СЏС‚РѕСЂР°."));
    initKnob(osc2Oct, "osc2Oct", "O2 OCT", u8("РћРєС‚Р°РІР° 2-РіРѕ РѕСЃС†РёР»Р»СЏС‚РѕСЂР° (-2..+2)."));
    initKnob(osc2Semi, "osc2Semi", "O2 SEMI", u8("РџРѕР»СѓС‚РѕРЅР° (Pitch) 2-РіРѕ РѕСЃС†РёР»Р»СЏС‚РѕСЂР° (РЅР°РїСЂРёРјРµСЂ, +7 РєРІРёРЅС‚Р°)."));
    initKnob(osc2Unison, "osc2Unison", "UNISON", u8("UNISON: РєРѕР»РёС‡РµСЃС‚РІРѕ РіРѕР»РѕСЃРѕРІ СѓРЅРёСЃРѕРЅР° (1вЂ“7)."));
    initKnob(osc2Level, "osc2Level", "O2 LVL", u8("Р“СЂРѕРјРєРѕСЃС‚СЊ 2-РіРѕ РѕСЃС†РёР»Р»СЏС‚РѕСЂР°."));
    initKnob(subLevel, "subLevel", "SUB LVL", u8("SUB: РјРѕС‰РЅС‹Р№ СЃР°Р±-Р±Р°СЃ СЂРѕРІРЅРѕ РЅР° -1 РѕРєС‚Р°РІСѓ РЅРёР¶Рµ РЅРѕС‚С‹."));
    initKnob(entropyAmt, "entropyAmt", "ENTROPY", u8("ENTROPY: Р¶РёРІР°СЏ РјРёРєСЂРѕ-СЂР°СЃСЃС‚СЂРѕР№РєР° Р°РЅР°Р»РѕРіРѕРІС‹С… РєРѕРјРїРѕРЅРµРЅС‚РѕРІ РґР»СЏ РєР°Р¶РґРѕР№ РЅРѕС‚С‹."));
    initKnob(gravityAmt, "gravityAmt", "GRAVITY", u8("GRAVITY: РіСЂР°РІРёС‚Р°С†РёРѕРЅРЅРѕРµ РїСЂРѕСЃРµРґР°РЅРёРµ РїРёС‚С‡Р° Рё РѕСЂР±РёС‚ РѕС‚ СЌРЅРµСЂРіРёРё Р±Р°СЃР°."));
    initKnob(ghostVoxAmt, "ghostVoxAmt", "GHOST", u8("GHOST VOX: РіР°СЂРјРѕРЅРёС‡РµСЃРєРёРµ РєРІРёРЅС‚РѕРІС‹Рµ Рё РѕРєС‚Р°РІРЅС‹Рµ С‚РµРЅРё РІ С…РІРѕСЃС‚Рµ РЅРѕС‚С‹."));

    initCombo(lfoTargetBox, "lfoTarget", { "Pitch", "Cutoff", "Morph" }, u8("Р¦РµР»СЊ LFO: Pitch (РІРёР±СЂР°С‚Рѕ), Cutoff (СЃСЂРµР· С„РёР»СЊС‚СЂР°) РёР»Рё Morph (С„РѕСЂРјР° РІРѕР»РЅС‹)."));
    initCombo(lfoShapeBox, "lfoShape", { "Sine", "Square", "Ramp", "S&H", "BioChaos" }, u8("Р¤РѕСЂРјР° LFO: РїР»Р°РІРЅР°СЏ, СЃС‚СѓРїРµРЅСЊРєРё S&H-РіР»РёС‚С‡Р° РёР»Рё Р¶РёРІРѕР№ Р°С‚С‚СЂР°РєС‚РѕСЂ Р›РѕСЂРµРЅС†Р° (BioChaos)."));
    initKnob(lfoRate, "lfoRate", "RATE", u8("RATE: СЃРєРѕСЂРѕСЃС‚СЊ LFO."));
    initKnob(lfoDepth, "lfoDepth", "DEPTH", u8("DEPTH: РіР»СѓР±РёРЅР° РІРѕР·РґРµР№СЃС‚РІРёСЏ LFO РЅР° РІС‹Р±СЂР°РЅРЅСѓСЋ С†РµР»СЊ."));
    initKnob(pitchPunch, "pitchPunch", "P.PUNCH", u8("PITCH PUNCH: СѓРґР°СЂ РїРёС‚С‡Р° РІ СЃР°РјРѕРј РЅР°С‡Р°Р»Рµ РЅРѕС‚С‹ (РґР»СЏ 808 Рё Р»Р°Р·РµСЂРѕРІ)."));
    initKnob(pumpAmt, "pumpAmt", "PUMP", u8("AUTO PUMP: РІСЃС‚СЂРѕРµРЅРЅС‹Р№ BPM-СЃР°Р№РґС‡РµР№РЅ РїРѕРґ С‚РµРјРї FL Studio."));
    initKnob(noiseLevel, "noiseLevel", "NOISE", u8("NOISE: С€СѓРј РєР°СЃСЃРµС‚С‹ Рё РІРѕР·РґСѓС…Р° (РёР»Рё РѕСЃРЅРѕРІР° РґР»СЏ Comb-СЃС‚СЂСѓРЅ)."));
    initKnob(vhsDrift, "vhsDrift", "DRIFT", u8("VHS DRIFT: РїР»Р°РІР°РЅРёРµ РїРёС‚С‡Р° СЃС‚Р°СЂРѕР№ РІРёРґРµРѕРєР°СЃСЃРµС‚С‹."));

    initCombo(arpModeBox, "arpMode", { "ArpOff", "Stutter", "ChipArp", "Chaos" }, u8("Р РµР¶РёРј СЂРёС‚РјР°: Off, Stutter (СЂСѓР±РєР° Р°РєРєРѕСЂРґР°), ChipArp (8-Р±РёС‚ Р°СЂРї), Chaos."));
    initKnob(arpRate, "arpRate", "ARP SPD", u8("ARP SPD: СЃРµС‚РєР° С€Р°РіРѕРІ (1/4, 1/8, С‚СЂРёРѕР»Рё 1/8T, 1/16, 1/16T, 1/32)."));
    addAndMakeVisible(stepMatrix);
    registerHelp(&stepMatrix, u8("8-С€Р°РіРѕРІС‹Р№ СЃРµРєРІРµРЅСЃРѕСЂ: РєР»РёРєР°Р№С‚Рµ РїРѕ РєСѓР±РёРєР°Рј (-- РїР°СѓР·Р°, ON СѓРґР°СЂ, +12 РѕРєС‚Р°РІР°, 2X РґРІРѕР№РЅР°СЏ РґСЂРѕР±СЊ)."));

    addAndMakeVisible(xyPad);
    registerHelp(&xyPad, u8("AETHER XY: РїРѕ X (CHAOS) вЂ” Р°РіСЂРµСЃСЃРёСЏ Рё РїРµСЂРµРіСЂСѓР·, РїРѕ Y (DIMENSION) вЂ” РєРѕСЃРјРѕСЃ Рё С€РёРјРјРµСЂ."));

    initToggle(distBtn, "distOn", u8("Р’РєР»СЋС‡Р°РµС‚ Р±Р»РѕРє РїРµСЂРµРіСЂСѓР·Р°/СЃР°С‚СѓСЂР°С†РёРё."));
    initCombo(distModeBox, "distMode", { "Tube", "Diode", "Rectify", "FoldX", "BitClip" }, u8("Р РµР¶РёРј РїРµСЂРµРіСЂСѓР·Р° (2x РѕРІРµСЂСЃСЌРјРїР»РёРЅРі): Р»Р°РјРїР°, РґРёРѕРґ RAT, Rectify, FoldX, BitClip."));
    initKnob(distDrive, "distDrive", "DRIVE", u8("РЎРёР»Р° РїРµСЂРµРіСЂСѓР·Р°."));
    initKnob(distMix, "distMix", "D.MIX", u8("Р‘Р°Р»Р°РЅСЃ РјРµР¶РґСѓ С‡РёСЃС‚С‹Рј Р±Р°СЃРѕРј Рё РїРµСЂРµРіСЂСѓР·РѕРј."));
    initKnob(crushAmount, "crushAmount", "CRUSH", u8("Bitcrusher: РїРѕРЅРёР¶РµРЅРёРµ Р±РёС‚РЅРѕСЃС‚Рё РґР»СЏ 8-Р±РёС‚РЅРѕРіРѕ Р·РІСѓРєР°."));
    initKnob(lowCutHz, "lowCutHz", "LOW CUT", u8("РЎСЂРµР· Р»РёС€РЅРµРіРѕ РЅРёР·Р° (20вЂ“250 Hz), С‡С‚РѕР±С‹ РЅРµ РјРµС€Р°С‚СЊ 808-Р№ Р±РѕС‡РєРµ."));

    initToggle(chorusBtn, "chorusOn", u8("Р’РєР»СЋС‡Р°РµС‚ СЃС‚РµСЂРµРѕ-С…РѕСЂСѓСЃ."));
    initKnob(chorusMix, "chorusMix", "MIX", u8("Р“СЂРѕРјРєРѕСЃС‚СЊ СЃС‚РµСЂРµРѕ-С…РѕСЂСѓСЃР°."));
    initKnob(chorusRate, "chorusRate", "RATE", u8("РЎРєРѕСЂРѕСЃС‚СЊ РјРѕРґСѓР»СЏС†РёРё С…РѕСЂСѓСЃР°."));
    initKnob(chorusDepth, "chorusDepth", "DEPTH", u8("Р“Р»СѓР±РёРЅР° С…РѕСЂСѓСЃР°."));

    initToggle(doublerBtn, "doublerOn", u8("DOUBLER: СѓРґРІРѕРёС‚РµР»СЊ РіРѕР»РѕСЃР° СЃРѕ СЃС‚РµСЂРµРѕ-СЂР°СЃС€РёСЂРµРЅРёРµРј. Р”РµР»Р°РµС‚ Р·РІСѓРє С‚РѕР»С‰Рµ Рё С€РёСЂРµ."));
    initKnob(doublerWidth, "doublerWidth", "WIDTH", u8("Р РђР—РњРђРҐ: РЅР°СЃРєРѕР»СЊРєРѕ С€РёСЂРѕРєРѕ РєРѕРїРёРё СЂР°Р·РІРѕРґСЏС‚СЃСЏ РїРѕ СЃС‚РµСЂРµРѕ."));
    initKnob(doublerDtune, "doublerDtune", "DTUNE", u8("Р РђРЎРЎРўР РћР™РљРђ РєРѕРїРёР№ РІ С†РµРЅС‚Р°С…. Р‘РѕР»СЊС€Рµ вЂ” С€РёСЂРµ Рё С…РѕСЂРѕРІРµРµ."));
    initKnob(doublerMix, "doublerMix", "D.MIX", u8("Р‘Р°Р»Р°РЅСЃ СЃСѓС…РѕРіРѕ Рё СѓРґРІРѕРµРЅРЅРѕРіРѕ СЃРёРіРЅР°Р»Р°."));

    initToggle(delayBtn, "delayOn", u8("Р’РєР»СЋС‡Р°РµС‚ СЌС…Рѕ (Delay)."));
    initCombo(delayModeBox, "delayMode", { "Stereo", "PingPong", "GhostOct" }, u8("Р РµР¶РёРј СЌС…Р°: Stereo, PingPong (РїРµСЂРµРєСЂС‘СЃС‚РЅРѕРµ) РёР»Рё GhostOct (СѓР»РµС‚Р°РµС‚ РЅР° РѕРєС‚Р°РІСѓ РІРІРµСЂС…)."));
    initKnob(delayTime, "delayTime", "TIME", u8("Р’СЂРµРјСЏ Р·Р°РґРµСЂР¶РєРё СЌС…Р°."));
    initKnob(delayFb, "delayFeedback", "F.BACK", u8("РљРѕР»РёС‡РµСЃС‚РІРѕ РїРѕРІС‚РѕСЂРѕРІ СЌС…Р° (Feedback)."));
    initKnob(delayMix, "delayMix", "MIX", u8("Р“СЂРѕРјРєРѕСЃС‚СЊ СЌС…Р°."));

    initToggle(reverbBtn, "reverbOn", u8("Р’РєР»СЋС‡Р°РµС‚ СЂРµРІРµСЂР±РµСЂР°С‚РѕСЂ."));
    initToggle(freezeBtn, "reverbFreeze", u8("FREEZE: Р·Р°РјРѕСЂР°Р¶РёРІР°РµС‚ С…РІРѕСЃС‚ СЂРµРІРµСЂР±Р° РІ Р±РµСЃРєРѕРЅРµС‡РЅС‹Р№ РґСЂРѕРЅ."));
    initKnob(reverbSize, "reverbSize", "SIZE", u8("Р Р°Р·РјРµСЂ РїРѕРјРµС‰РµРЅРёСЏ СЂРµРІРµСЂР±РµСЂР°С‚РѕСЂР°."));
    initKnob(reverbMix, "reverbMix", "MIX", u8("Р“СЂРѕРјРєРѕСЃС‚СЊ СЂРµРІРµСЂР±РµСЂР°С†РёРё."));
    initKnob(haloAmt, "haloAmt", "HALO", u8("SHIMMER HALO: РґРѕР±Р°РІР»СЏРµС‚ РІ СЂРµРІРµСЂР± РѕРєС‚Р°РІРЅРѕРµ РєРѕСЃРјРёС‡РµСЃРєРѕРµ СЃРІРµС‡РµРЅРёРµ."));

    initKnob(masterGain, "masterGain", "MASTER", u8("РћР±С‰Р°СЏ РіСЂРѕРјРєРѕСЃС‚СЊ СЃРёРЅС‚РµР·Р°С‚РѕСЂР° СЃ РјСЏРіРєРёРј Р»РёРјРёС‚РµСЂРѕРј."));
    initToggle(tapeStopBtn, "tapeStop", u8("TAPE STOP: СЌС„С„РµРєС‚ Р·Р°Р¶С‘РІР°РЅРЅРѕР№ РєР°СЃСЃРµС‚С‹ вЂ” СЂРѕРЅСЏРµС‚ РїРёС‚С‡ РІ РЅРѕР»СЊ (РјРѕР¶РЅРѕ Р°РІС‚РѕРјР°С‚РёР·РёСЂРѕРІР°С‚СЊ)."));

    addMouseListener(this, true);
    setSidePage(SidePage::Main);
    syncPresetUI();
    disableFocusRecursive(*this);
    startTimerHz(30);
    setSize(1160, showKeyboard ? 790 : 720);
}

WitchSynthAudioProcessorEditor::~WitchSynthAudioProcessorEditor()
{
    removeMouseListener(this);
    stopTimer();
    setLookAndFeel(nullptr);
}

void WitchSynthAudioProcessorEditor::buildChassisTexture()
{
    const int W = juce::jmax(100, getWidth());
    const int H = juce::jmax(100, getHeight());
    chassisCache = juce::Image(juce::Image::RGB, W, H, false);
    juce::Graphics tg(chassisCache);

    tg.setGradientFill(juce::ColourGradient(juce::Colour(0xff161222), 0.0f, 0.0f,
                                            juce::Colour(0xff09070f), 0.0f, static_cast<float>(H), false));
    tg.fillAll();

    juce::Random rng(20372040);
    for (int y = 0; y < H; ++y)
    {
        const float streak = (rng.nextFloat() - 0.5f) * 0.032f;
        tg.setColour(streak > 0.0f ? juce::Colours::white.withAlpha(streak) : juce::Colours::black.withAlpha(-streak * 1.4f));
        tg.drawHorizontalLine(y, 0.0f, static_cast<float>(W));
    }
    for (int i = 0; i < 5000; ++i)
    {
        const int rx = rng.nextInt(W);
        const int ry = rng.nextInt(H);
        tg.setColour(rng.nextBool() ? juce::Colour(0x0cffffff) : juce::Colour(0x14000000));
        tg.fillRect(rx, ry, 2, 1);
    }

    auto drawRackEar = [&](int ex)
    {
        tg.setGradientFill(juce::ColourGradient(juce::Colour(0xff231d33), static_cast<float>(ex), 0.0f,
                                                juce::Colour(0xff120e1c), static_cast<float>(ex + 12), 0.0f, false));
        tg.fillRect(ex, 0, 12, H);
        tg.setColour(juce::Colour(0x28ffffff));
        tg.drawVerticalLine(ex == 0 ? 11 : ex, 0.0f, static_cast<float>(H));
    };
    drawRackEar(0);
    drawRackEar(W - 12);

    auto drawScrew = [&](float sx, float sy)
    {
        tg.setColour(juce::Colour(0xbb000000));
        tg.fillEllipse(sx - 5.5f, sy - 4.5f, 11.0f, 11.0f);
        tg.setGradientFill(juce::ColourGradient(juce::Colour(0xff6b6380), sx - 4.0f, sy - 4.0f,
                                                juce::Colour(0xff211c2b), sx + 4.0f, sy + 4.0f, true));
        tg.fillEllipse(sx - 5.0f, sy - 5.0f, 10.0f, 10.0f);
        tg.setColour(juce::Colour(0xff120e1a));
        tg.drawLine(sx - 3.2f, sy - 2.0f, sx + 3.2f, sy + 2.0f, 1.4f);
        tg.drawLine(sx - 2.0f, sy + 3.2f, sx + 2.0f, sy - 3.2f, 1.1f);
    };
    drawScrew(21.0f, 22.0f);
    drawScrew(static_cast<float>(W) - 21.0f, 22.0f);
    drawScrew(21.0f, static_cast<float>(H) - 22.0f);
    drawScrew(static_cast<float>(W) - 21.0f, static_cast<float>(H) - 22.0f);
}

void WitchSynthAudioProcessorEditor::registerHelp(juce::Component* c, const juce::String& ruHelp)
{
    if (c != nullptr) helpMap[c] = ruHelp;
}

void WitchSynthAudioProcessorEditor::mouseEnter(const juce::MouseEvent& e)
{
    auto* c = e.eventComponent;
    while (c != nullptr && c != this)
    {
        auto it = helpMap.find(c);
        if (it != helpMap.end())
        {
            if (activeHelpText != it->second)
            {
                activeHelpText = it->second;
                repaint(16, getHeight() - 40, 1128, 34);
            }
            return;
        }
        c = c->getParentComponent();
    }
}

void WitchSynthAudioProcessorEditor::mouseExit(const juce::MouseEvent&) {}

void WitchSynthAudioProcessorEditor::initKnob(juce::Slider& s, const juce::String& id, const juce::String& name, const juce::String& ruHelp, bool attachLbl)
{
    s.setName(name);
    s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 13);
    s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff090611));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    s.setColour(juce::Slider::textBoxTextColourId, Colors::glow);
    s.setWantsKeyboardFocus(false);
    s.setMouseClickGrabsKeyboardFocus(false);
    addAndMakeVisible(s);
    sliderAtts.push_back(std::make_unique<SliderAtt>(audioProcessor.apvts, id, s));
    registerHelp(&s, ruHelp);

    if (attachLbl)
    {
        auto lbl = std::make_unique<juce::Label>("", name);
        lbl->setJustificationType(juce::Justification::centred);
        lbl->setColour(juce::Label::textColourId, Colors::subText);
        lbl->setFont(juce::FontOptions(9.5f, juce::Font::bold));
        lbl->attachToComponent(&s, false);
        lbl->setWantsKeyboardFocus(false);
        lbl->setMouseClickGrabsKeyboardFocus(false);
        addAndMakeVisible(*lbl);
        registerHelp(lbl.get(), ruHelp);
        knobLabels[&s] = std::move(lbl);
    }
}

void WitchSynthAudioProcessorEditor::initCombo(juce::ComboBox& c, const juce::String& id, const juce::StringArray& items, const juce::String& ruHelp)
{
    for (int i = 0; i < items.size(); ++i)
        c.addItem(items[i], i + 1);
    c.setWantsKeyboardFocus(false);
    c.setMouseClickGrabsKeyboardFocus(false);
    addAndMakeVisible(c);
    comboAtts.push_back(std::make_unique<ComboAtt>(audioProcessor.apvts, id, c));
    registerHelp(&c, ruHelp);
}

void WitchSynthAudioProcessorEditor::initToggle(juce::ToggleButton& b, const juce::String& id, const juce::String& ruHelp)
{
    b.setWantsKeyboardFocus(false);
    b.setMouseClickGrabsKeyboardFocus(false);
    addAndMakeVisible(b);
    btnAtts.push_back(std::make_unique<BtnAtt>(audioProcessor.apvts, id, b));
    registerHelp(&b, ruHelp);
}

void WitchSynthAudioProcessorEditor::showCtrl(juce::Component& c, bool visible)
{
    c.setVisible(visible);
    if (auto it = knobLabels.find(&c); it != knobLabels.end())
        it->second->setVisible(visible);
}

void WitchSynthAudioProcessorEditor::setSidePage(SidePage page)
{
    currentPage = page;
    auto styleTab = [&](juce::TextButton& b, bool active)
    {
        b.setColour(juce::TextButton::buttonColourId, active ? Colors::purple : juce::Colour(0xff1b152a));
        b.setColour(juce::TextButton::textColourOffId, active ? juce::Colours::white : Colors::glow);
    };
    styleTab(pageMainBtn, currentPage == SidePage::Main);
    styleTab(pageOscBtn,  currentPage == SidePage::Osc);
    styleTab(pageArpBtn,  currentPage == SidePage::Arp);
    styleTab(pageXyBtn,   currentPage == SidePage::XY);

    updateVisibility();
    repaint();
}

void WitchSynthAudioProcessorEditor::updateVisibility()
{
    const bool m = (currentPage == SidePage::Main);
    const bool o = (currentPage == SidePage::Osc);
    const bool a = (currentPage == SidePage::Arp);
    const bool x = (currentPage == SidePage::XY);

    showCtrl(filterTypeBox, m); showCtrl(filterCutoff, m); showCtrl(filterReso, m); showCtrl(filterDrive, m);
    showCtrl(filterEnvAmt, m); showCtrl(filterDecay, m); showCtrl(filterKeyTrack, m); showCtrl(tiltAmt, m); showCtrl(stereoWidth, m);
    showCtrl(polyModeBox, m); showCtrl(morphPresetBox, m); showCtrl(ampA, m); showCtrl(ampD, m); showCtrl(ampS, m);
    showCtrl(ampR, m); showCtrl(glideTime, m); showCtrl(snapAmt, m); showCtrl(airAmt, m); showCtrl(morphAmt, m);

    showCtrl(osc1WaveBox, o); showCtrl(osc2WaveBox, o);
    showCtrl(osc1Oct, o); showCtrl(osc1Semi, o); showCtrl(pwmWidth, o); showCtrl(xModAmt, o); showCtrl(osc1Detune, o); showCtrl(osc1Level, o);
    showCtrl(osc2Oct, o); showCtrl(osc2Semi, o); showCtrl(osc2Unison, o); showCtrl(osc2Level, o);
    showCtrl(subLevel, o); showCtrl(entropyAmt, o); showCtrl(gravityAmt, o); showCtrl(ghostVoxAmt, o);

    showCtrl(lfoTargetBox, a); showCtrl(lfoShapeBox, a); showCtrl(lfoRate, a); showCtrl(lfoDepth, a);
    showCtrl(pitchPunch, a); showCtrl(pumpAmt, a); showCtrl(noiseLevel, a); showCtrl(vhsDrift, a);
    showCtrl(arpModeBox, a); showCtrl(arpRate, a); showCtrl(stepMatrix, a);

    showCtrl(xyPad, x);
}

void WitchSynthAudioProcessorEditor::updateLikeButtonVisual()
{
    const bool fav = audioProcessor.isFavorite(audioProcessor.getCurrentPresetIndex());
    likeBtn.setButtonText(fav ? u8("♥") : u8("♡"));
    likeBtn.setColour(juce::TextButton::buttonColourId, fav ? Colors::hotPink : juce::Colour(0xff1b152a));
    likeBtn.setColour(juce::TextButton::textColourOffId, fav ? juce::Colours::white : Colors::glow);
}

void WitchSynthAudioProcessorEditor::rebuildPresetListForCategory(int uiCategoryIdx)
{
    presetModel.items.clear();
    visiblePresetIndices.clear();
    const auto& presets = getAllPresets();

    if (uiCategoryIdx == 0)
    {
        auto favs = audioProcessor.getFavoriteIndices();
        if (favs.empty())
            presetModel.items.push_back(u8("-- РќРµС‚ Р»Р°Р№РєРѕРІ (РЅР°Р¶РјРё ♡) --"));
        else
        {
            for (int pIdx : favs)
            {
                presetModel.items.push_back(u8("♥ ") + presets[static_cast<size_t>(pIdx)].name);
                visiblePresetIndices.push_back(pIdx);
            }
        }
    }
    else
    {
        const int factoryCat = uiCategoryIdx - 1;
        for (int i = 0; i < static_cast<int>(presets.size()); ++i)
        {
            if (presets[static_cast<size_t>(i)].folderIndex == factoryCat)
            {
                const juce::String prefix = audioProcessor.isFavorite(i) ? u8("♥ ") : "";
                presetModel.items.push_back(prefix + presets[static_cast<size_t>(i)].name);
                visiblePresetIndices.push_back(i);
            }
        }
    }

    presetList.updateContent();
    presetList.repaint();
    disableFocusRecursive(categoryList);
    disableFocusRecursive(presetList);
}

void WitchSynthAudioProcessorEditor::syncPresetUI()
{
    const int idx = audioProcessor.getCurrentPresetIndex();
    lastSeenPreset = idx;
    const auto& presets = getAllPresets();
    if (idx >= 0 && idx < static_cast<int>(presets.size()))
    {
        const bool stayInFavs = (selectedUiCategory == 0) && audioProcessor.isFavorite(idx);
        selectedUiCategory = stayInFavs ? 0 : (presets[static_cast<size_t>(idx)].folderIndex + 1);
        categoryList.selectRow(selectedUiCategory, false, true);
        rebuildPresetListForCategory(selectedUiCategory);

        for (size_t row = 0; row < visiblePresetIndices.size(); ++row)
        {
            if (visiblePresetIndices[row] == idx)
            {
                presetList.selectRow(static_cast<int>(row), false, true);
                break;
            }
        }
    }
    updateLikeButtonVisual();
    repaint(330, 12, 500, 382);
}

void WitchSynthAudioProcessorEditor::randomizeWitchPreset()
{
    auto& rng = juce::Random::getSystemRandom();
    auto setP = [this](const char* id, float val)
    {
        if (auto* param = audioProcessor.apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(val));
    };

    setP("osc1Wave", static_cast<float>(rng.nextInt(8)));
    setP("osc1Oct", static_cast<float>(rng.nextInt(3) - 1));
    setP("osc1Detune", 0.15f + rng.nextFloat() * 0.65f);
    setP("pwmWidth", 0.15f + rng.nextFloat() * 0.65f);
    setP("xModAmt", rng.nextFloat() < 0.35f ? rng.nextFloat() * 0.35f : 0.04f);
    setP("osc2Wave", static_cast<float>(rng.nextInt(8)));
    setP("osc2Oct", static_cast<float>(rng.nextInt(3) - 1));
    setP("osc2Unison", static_cast<float>(2 + rng.nextInt(6)));
    setP("subLevel", rng.nextFloat() * 0.75f);
    setP("entropyAmt", 0.15f + rng.nextFloat() * 0.45f);
    setP("gravityAmt", rng.nextFloat() * 0.45f);
    setP("ghostVoxAmt", rng.nextFloat() * 0.45f);
    setP("crushAmount", rng.nextFloat() < 0.35f ? rng.nextFloat() * 0.35f : 0.0f);
    setP("tiltAmt", rng.nextFloat() * 0.5f - 0.15f);
    const int fPick = rng.nextInt(6);
    setP("filterType", static_cast<float>(fPick == 1 ? 5 : fPick));
    setP("filterCutoff", 750.0f + rng.nextFloat() * 6500.0f);
    setP("filterReso", 1.5f + rng.nextFloat() * 5.0f);
    setP("filterKeyTrack", 0.6f + rng.nextFloat() * 0.4f);
    setP("filterEnvAmt", 0.15f + rng.nextFloat() * 0.55f);
    setP("pitchPunch", rng.nextFloat() < 0.4f ? 0.1f + rng.nextFloat() * 0.4f : 0.05f);
    setP("snapAmt", rng.nextFloat() * 0.55f);
    setP("airAmt", 0.35f + rng.nextFloat() * 0.45f);
    setP("lfoRate", 0.8f + rng.nextFloat() * 6.0f);
    setP("lfoDepth", 0.1f + rng.nextFloat() * 0.40f);
    setP("lfoShape", static_cast<float>(rng.nextInt(5)));
    setP("arpMode", rng.nextFloat() < 0.30f ? static_cast<float>(1 + rng.nextInt(3)) : 0.0f);
    setP("arpRate", static_cast<float>(rng.nextInt(6)));
    for (int s = 0; s < 8; ++s)
        setP(("seq" + juce::String(s)).toRawUTF8(), static_cast<float>(rng.nextFloat() < 0.2f ? 0 : (1 + rng.nextInt(3))));
    setP("distOn", 1.0f);
    setP("distMode", static_cast<float>(rng.nextInt(5)));
    setP("distDrive", 0.25f + rng.nextFloat() * 0.50f);
    setP("delayMode", static_cast<float>(rng.nextInt(3)));
    setP("reverbOn", 1.0f);
    setP("haloAmt", rng.nextFloat() * 0.65f);
}

void WitchSynthAudioProcessorEditor::timerCallback()
{
    if (lastSeenPreset != audioProcessor.getCurrentPresetIndex())
        syncPresetUI();

    const float rawPeak = audioProcessor.peakLevel.load(std::memory_order_relaxed);
    smoothedPeak = rawPeak > smoothedPeak ? rawPeak : smoothedPeak * 0.85f;

    repaint(640, 322, 180, 62);
    repaint(1108, 442, 22, 146);
    if (currentPage == SidePage::Main)
    {
        repaint(26, 66, 286, 60);
        repaint(848, 66, 286, 60);
    }
    else if (currentPage == SidePage::Arp) stepMatrix.repaint();
    else if (currentPage == SidePage::XY)  xyPad.repaint();
}

void WitchSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    if (!chassisCache.isNull())
        g.drawImageAt(chassisCache, 0, 0);
    else
        g.fillAll(Colors::bg);

    auto drawHardwarePanel = [&](int x, int y, int w, int h, const juce::String& title)
    {
        const juce::Rectangle<float> r(static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h));
        g.setColour(juce::Colour(0x90000000));
        g.fillRoundedRectangle(r.translated(1.5f, 2.5f), 6.0f);

        g.setGradientFill(juce::ColourGradient(Colors::panelTop.withAlpha(0.92f), 0.0f, r.getY(),
                                               Colors::panelBot.withAlpha(0.95f), 0.0f, r.getBottom(), false));
        g.fillRoundedRectangle(r, 6.0f);

        g.setColour(juce::Colour(0x22ffffff));
        g.drawHorizontalLine(y + 1, r.getX() + 4.0f, r.getRight() - 4.0f);
        g.setColour(Colors::border);
        g.drawRoundedRectangle(r, 6.0f, 1.2f);

        g.setColour(juce::Colour(0x35000000));
        g.fillRoundedRectangle(r.getX() + 2.0f, r.getY() + 2.0f, r.getWidth() - 4.0f, 22.0f, 4.0f);
        g.setColour(Colors::neon);
        g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
        g.drawText(title, x + 12, y + 5, w - 24, 16, juce::Justification::left);
    };

    juce::String leftTitle = "FILTER & TONE";
    juce::String rightTitle = "AMP, ADSR & MORPH A<->B";
    if (currentPage == SidePage::Osc) { leftTitle = "OSC 1 & OSC 2"; rightTitle = "SUB & 2040 PHYSICS"; }
    else if (currentPage == SidePage::Arp) { leftTitle = "BIO-LFO, PUMP & TAPE"; rightTitle = "ARP & 8-STEP RATCHET"; }
    else if (currentPage == SidePage::XY) { leftTitle = "AETHER CORE (XY)"; rightTitle = "2040 MACRO MATRIX"; }

    drawHardwarePanel(16, 12, 306, 382, leftTitle);
    drawHardwarePanel(838, 12, 306, 382, rightTitle);

    if (currentPage == SidePage::Main)
    {
        const juce::Rectangle<float> fRect(28.0f, 68.0f, 282.0f, 56.0f);
        g.setColour(Colors::lcdBg);
        g.fillRoundedRectangle(fRect, 4.0f);
        g.setColour(Colors::border);
        g.drawRoundedRectangle(fRect, 4.0f, 1.0f);

        const int fType = static_cast<int>(audioProcessor.apvts.getRawParameterValue("filterType")->load());
        const float cutoff = audioProcessor.apvts.getRawParameterValue("filterCutoff")->load();
        const float reso = audioProcessor.apvts.getRawParameterValue("filterReso")->load();
        const float tilt = audioProcessor.apvts.getRawParameterValue("tiltAmt")->load();
        const float cutNorm = juce::jlimit(0.05f, 0.95f, (std::log2(cutoff / 20.0f) / 10.0f));
        const float qPeak = juce::jlimit(0.0f, 0.72f, (reso - 0.5f) / 14.0f);

        juce::Path fCurve, fFill;
        const int steps = 72;
        for (int i = 0; i <= steps; ++i)
        {
            const float xN = static_cast<float>(i) / static_cast<float>(steps);
            float mag = 0.52f;
            const float dist = xN - cutNorm;
            if (fType == 0 || fType == 5)
            {
                const float slope = fType == 5 ? 18.0f : 11.0f;
                mag = dist < 0.0f ? 0.56f : 0.56f / (1.0f + std::pow(dist * slope, 2.0f));
                mag += qPeak * std::exp(-dist * dist * 160.0f);
            }
            else if (fType == 1)
            {
                mag = dist > 0.0f ? 0.56f : 0.56f / (1.0f + std::pow(-dist * 12.0f, 2.0f));
                mag += qPeak * std::exp(-dist * dist * 160.0f);
            }
            else if (fType == 2)
            {
                mag = (0.28f + qPeak * 0.7f) * std::exp(-dist * dist * 65.0f);
            }
            else if (fType == 3)
            {
                const float p1 = cutNorm * 0.8f;
                const float p2 = juce::jmin(0.92f, cutNorm + 0.22f);
                mag = 0.16f + (0.45f + qPeak * 0.45f) * (std::exp(-(xN - p1) * (xN - p1) * 180.0f) + 0.8f * std::exp(-(xN - p2) * (xN - p2) * 180.0f));
            }
            else
            {
                mag = 0.30f + (0.25f + qPeak * 0.45f) * (0.5f + 0.5f * std::cos((xN - cutNorm) * 45.0f));
            }
            mag = juce::jlimit(0.04f, 0.94f, mag + (xN - 0.5f) * tilt * 0.25f);
            const float px = fRect.getX() + 3.0f + xN * (fRect.getWidth() - 6.0f);
            const float py = fRect.getBottom() - 4.0f - mag * (fRect.getHeight() - 8.0f);
            if (i == 0) { fCurve.startNewSubPath(px, py); fFill.startNewSubPath(px, fRect.getBottom() - 2.0f); fFill.lineTo(px, py); }
            else { fCurve.lineTo(px, py); fFill.lineTo(px, py); }
        }
        fFill.lineTo(fRect.getRight() - 3.0f, fRect.getBottom() - 2.0f);
        fFill.closeSubPath();
        g.setColour(Colors::purple.withAlpha(0.24f));
        g.fillPath(fFill);
        g.setColour(Colors::glow);
        g.strokePath(fCurve, juce::PathStrokeType(1.6f));

        const juce::Rectangle<float> eRect(850.0f, 68.0f, 282.0f, 56.0f);
        g.setColour(Colors::lcdBg);
        g.fillRoundedRectangle(eRect, 4.0f);
        g.setColour(Colors::border);
        g.drawRoundedRectangle(eRect, 4.0f, 1.0f);

        const float a = std::sqrt(audioProcessor.apvts.getRawParameterValue("ampAttack")->load() / 4.0f);
        const float d = std::sqrt(audioProcessor.apvts.getRawParameterValue("ampDecay")->load() / 4.0f);
        const float s = audioProcessor.apvts.getRawParameterValue("ampSustain")->load();
        const float r = std::sqrt(audioProcessor.apvts.getRawParameterValue("ampRelease")->load() / 6.0f);

        const float ew = eRect.getWidth() - 10.0f;
        const float eh = eRect.getHeight() - 10.0f;
        const float x0 = eRect.getX() + 5.0f;
        const float yBot = eRect.getBottom() - 4.0f;
        const float xA = x0 + juce::jmax(0.02f, a * 0.28f) * ew;
        const float xD = xA + juce::jmax(0.04f, d * 0.28f) * ew;
        const float xS = xD + 0.22f * ew;
        const float xR = juce::jmin(eRect.getRight() - 5.0f, xS + juce::jmax(0.04f, r * 0.28f) * ew);
        const float yS = yBot - s * eh;

        juce::Path envP, envFill;
        envP.startNewSubPath(x0, yBot);
        envP.lineTo(xA, yBot - eh);
        envP.lineTo(xD, yS);
        envP.lineTo(xS, yS);
        envP.lineTo(xR, yBot);
        envFill = envP;
        envFill.closeSubPath();
        g.setColour(Colors::purple.withAlpha(0.24f));
        g.fillPath(envFill);
        g.setColour(Colors::neon);
        g.strokePath(envP, juce::PathStrokeType(1.7f));
    }
    else if (currentPage == SidePage::XY)
    {
        g.setColour(Colors::subText);
        g.setFont(juce::FontOptions(12.5f));
        g.drawFittedText(u8("AETHER CORE (XY) СѓРїСЂР°РІР»СЏРµС‚ Р·РІСѓРєРѕРј РІ СЂРµР°Р»СЊРЅРѕРј РІСЂРµРјРµРЅРё:\n\n"
                            "вЂў РћСЃСЊ X (CHAOS): РѕРґРЅРѕРІСЂРµРјРµРЅРЅРѕ РїРѕРІС‹С€Р°РµС‚ Р°РіСЂРµСЃСЃРёСЋ РїРµСЂРµРіСЂСѓР·Р°, "
                            "FM/Sync, СЃСЂРµР· С„РёР»СЊС‚СЂР° Рё Bitcrusher.\n\n"
                            "вЂў РћСЃСЊ Y (DIMENSION): СЂР°СЃРєСЂС‹РІР°РµС‚ СЂРµРІРµСЂР±РµСЂР°С†РёСЋ, Shimmer Halo, "
                            "СЌС…Рѕ, СЃС‚РµСЂРµРѕ-С€РёСЂРёРЅСѓ Рё Crystal Air.\n\n"
                            "РћР±Рµ РѕСЃРё РїРѕРґРґРµСЂР¶РёРІР°СЋС‚ Р·Р°РїРёСЃСЊ Р°РІС‚РѕРјР°С‚РёР·Р°С†РёРё РІ FL Studio!"),
                         854, 56, 278, 280, juce::Justification::topLeft, 12);
    }
    else if (currentPage == SidePage::Arp)
    {
        g.setColour(Colors::subText);
        g.setFont(juce::FontOptions(11.0f));
        g.drawText(u8("РљР»РёРєР°Р№ С€Р°РіРё РЅРёР¶Рµ: -- (РїР°СѓР·Р°) | ON | +12 | 2X (РґСЂРѕР±СЊ)"), 850, 178, 286, 18, juce::Justification::centred);
    }

    drawHardwarePanel(332, 12, 496, 382, "");

    g.setColour(Colors::neon);
    juce::Path tri;
    tri.addTriangle(354.0f, 18.0f, 343.0f, 42.0f, 365.0f, 42.0f);
    g.strokePath(tri, juce::PathStrokeType(1.7f));
    g.drawLine(354.0f, 25.0f, 354.0f, 39.0f, 1.7f);
    g.drawLine(348.0f, 31.0f, 360.0f, 31.0f, 1.7f);

    g.setColour(Colors::glow);
    g.setFont(juce::FontOptions(17.5f, juce::Font::bold));
    g.drawText("WITCHSYNTH", 374, 17, 170, 24, juce::Justification::left);
    g.setColour(Colors::subText);
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.drawText("v10.0 HARDWARE // 100 PRESETS", 548, 21, 268, 18, juce::Justification::right);

    const juce::Rectangle<float> lcdFrame(344.0f, 80.0f, 472.0f, 236.0f);
    g.setColour(Colors::lcdBg);
    g.fillRoundedRectangle(lcdFrame, 5.0f);

    g.setColour(Colors::purple.withAlpha(0.14f));
    for (int gx = 370; gx < 810; gx += 36)
        g.drawLine(static_cast<float>(gx), 104.0f, static_cast<float>(gx) + (static_cast<float>(gx) - 580.0f) * 0.25f, 290.0f, 0.8f);
    for (int gy = 130; gy < 290; gy += 32)
        g.drawHorizontalLine(gy, 346.0f, 814.0f);

    g.setColour(juce::Colour(0xdd18102b));
    g.fillRect(346, 82, 468, 20);
    g.setColour(Colors::neon);
    g.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    g.drawText("CATEGORY", 354, 83, 160, 18, juce::Justification::left);
    g.drawText("PRESET", 536, 83, 260, 18, juce::Justification::left);

    g.setColour(juce::Colour(0xee160f26));
    g.fillRect(346, 291, 468, 23);
    const int curIdx = audioProcessor.getCurrentPresetIndex();
    const auto& allP = getAllPresets();
    const juce::String curName = (curIdx >= 0 && curIdx < static_cast<int>(allP.size())) ? allP[static_cast<size_t>(curIdx)].name : "";
    g.setColour(Colors::glow);
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.drawText(curName, 354, 292, 340, 21, juce::Justification::left, true);
    g.setColour(Colors::subText);
    g.setFont(juce::FontOptions(10.5f));
    g.drawText(juce::String(curIdx + 1) + " / " + juce::String(allP.size()), 700, 292, 108, 21, juce::Justification::right);

    const juce::Rectangle<float> scopeRect(642.0f, 324.0f, 174.0f, 58.0f);
    g.setColour(Colors::lcdBg);
    g.fillRoundedRectangle(scopeRect, 4.0f);
    g.setColour(Colors::border);
    g.drawRoundedRectangle(scopeRect, 4.0f, 1.0f);

    juce::Path wave;
    const int writePos = audioProcessor.scopeWritePos.load(std::memory_order_relaxed);
    for (int i = 0; i < WitchSynthAudioProcessor::scopeSize; ++i)
    {
        const int idx = (writePos + i) % WitchSynthAudioProcessor::scopeSize;
        const float sample = juce::jlimit(-1.0f, 1.0f, audioProcessor.scopeBuffer[static_cast<size_t>(idx)]);
        const float px = scopeRect.getX() + 3.0f + (static_cast<float>(i) / static_cast<float>(WitchSynthAudioProcessor::scopeSize - 1)) * (scopeRect.getWidth() - 6.0f);
        const float py = scopeRect.getCentreY() - sample * (scopeRect.getHeight() * 0.42f);
        if (i == 0) wave.startNewSubPath(px, py);
        else wave.lineTo(px, py);
    }
    const auto scopeCol = smoothedPeak > 0.85f ? Colors::hotPink : Colors::purple;
    g.setColour(scopeCol.withAlpha(0.45f));
    g.strokePath(wave, juce::PathStrokeType(3.6f));
    g.setColour(Colors::glow);
    g.strokePath(wave, juce::PathStrokeType(1.4f));

    drawHardwarePanel(16,   404, 244, 266, "PREAMP / LO-FI");
    drawHardwarePanel(270,  404, 186, 266, "CHORUS & DOUBLER");
    drawHardwarePanel(466,  404, 250, 266, "DELAY");
    drawHardwarePanel(726,  404, 268, 266, "REVERB & SHIMMER");
    drawHardwarePanel(1004, 404, 140, 266, "OUTPUT");

    const juce::Rectangle<float> meterRect(1110.0f, 444.0f, 16.0f, 142.0f);
    g.setColour(Colors::lcdBg);
    g.fillRoundedRectangle(meterRect, 3.0f);
    g.setColour(Colors::border);
    g.drawRoundedRectangle(meterRect, 3.0f, 1.0f);

    const int numSegs = 14;
    const float segH = (meterRect.getHeight() - 6.0f) / static_cast<float>(numSegs);
    const int litSegs = static_cast<int>(std::round(juce::jlimit(0.0f, 1.0f, smoothedPeak) * static_cast<float>(numSegs)));
    for (int sgi = 0; sgi < numSegs; ++sgi)
    {
        const float sy = meterRect.getBottom() - 3.0f - static_cast<float>(sgi + 1) * segH;
        const bool lit = sgi < litSegs;
        const auto segCol = (sgi >= 11) ? Colors::hotPink : ((sgi >= 8) ? Colors::glow : Colors::neon);
        g.setColour(lit ? segCol : juce::Colour(0xff1a1329));
        g.fillRoundedRectangle(meterRect.getX() + 2.5f, sy + 1.0f, meterRect.getWidth() - 5.0f, segH - 2.0f, 1.5f);
    }

    const int hudY = getHeight() - 38;
    g.setColour(juce::Colour(0xff0b0813));
    g.fillRoundedRectangle(16.0f, static_cast<float>(hudY), 1128.0f, 30.0f, 4.0f);
    g.setColour(Colors::border);
    g.drawRoundedRectangle(16.0f, static_cast<float>(hudY), 1128.0f, 30.0f, 4.0f, 1.0f);
    g.setColour(Colors::glow);
    g.setFont(juce::FontOptions(12.2f));
    g.drawText(u8("РџРћР”РЎРљРђР—РљРђ: ") + activeHelpText, 24, hudY + 1, 1112, 28, juce::Justification::left, true);
}

void WitchSynthAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    const juce::Rectangle<float> lcdFrame(344.0f, 80.0f, 472.0f, 236.0f);

    juce::ColourGradient glassGrad(juce::Colour(0x26e0aaff), 344.0f, 80.0f,
                                   juce::Colour(0x048b3dff), 620.0f, 240.0f, false);
    glassGrad.addColour(0.45, juce::Colour(0x0a9d4edd));
    g.setGradientFill(glassGrad);
    g.fillRoundedRectangle(lcdFrame, 5.0f);

    g.setColour(juce::Colour(0xaa020106));
    g.drawRoundedRectangle(lcdFrame.reduced(0.5f), 5.0f, 2.5f);
    g.setColour(Colors::purple.withAlpha(0.75f));
    g.drawRoundedRectangle(lcdFrame, 5.0f, 1.2f);
    g.setColour(juce::Colour(0x35ffffff));
    g.drawHorizontalLine(81, 350.0f, 810.0f);
}

void WitchSynthAudioProcessorEditor::resized()
{
    if (chassisCache.isNull() || chassisCache.getWidth() != getWidth() || chassisCache.getHeight() != getHeight())
        buildChassisTexture();

    pageMainBtn.setBounds(344, 52, 115, 22);
    pageOscBtn.setBounds(463, 52, 115, 22);
    pageArpBtn.setBounds(582, 52, 115, 22);
    pageXyBtn.setBounds(701, 52, 115, 22);

    categoryList.setBounds(346, 104, 178, 185);
    presetList.setBounds(528, 104, 286, 185);

    prevBtn.setBounds(344, 324, 32, 26);
    nextBtn.setBounds(380, 324, 32, 26);
    likeBtn.setBounds(416, 324, 44, 26);
    rndBtn.setBounds(464, 324, 54, 26);
    initBtn.setBounds(522, 324, 54, 26);
    kbdBtn.setBounds(580, 324, 54, 26);
    lockFxBtn.setBounds(344, 356, 142, 26);
    panicBtn.setBounds(492, 356, 142, 26);

    filterTypeBox.setBounds(28, 40, 282, 22);
    filterCutoff.setBounds(26, 152, 66, 58);
    filterReso.setBounds(98, 152, 66, 58);
    filterDrive.setBounds(170, 152, 66, 58);
    filterEnvAmt.setBounds(242, 152, 66, 58);
    filterDecay.setBounds(26, 258, 66, 58);
    filterKeyTrack.setBounds(98, 258, 66, 58);
    tiltAmt.setBounds(170, 258, 66, 58);
    stereoWidth.setBounds(242, 258, 66, 58);

    polyModeBox.setBounds(850, 40, 88, 22);
    morphPresetBox.setBounds(944, 40, 188, 22);
    ampA.setBounds(848, 152, 66, 58);
    ampD.setBounds(920, 152, 66, 58);
    ampS.setBounds(992, 152, 66, 58);
    ampR.setBounds(1064, 152, 66, 58);
    glideTime.setBounds(848, 258, 66, 58);
    snapAmt.setBounds(920, 258, 66, 58);
    airAmt.setBounds(992, 258, 66, 58);
    morphAmt.setBounds(1064, 258, 66, 58);

    osc1WaveBox.setBounds(28, 42, 136, 24);
    osc2WaveBox.setBounds(172, 42, 136, 24);
    osc1Oct.setBounds(22, 102, 56, 60);
    osc1Semi.setBounds(82, 102, 56, 60);
    pwmWidth.setBounds(142, 102, 56, 60);
    xModAmt.setBounds(202, 102, 56, 60);
    osc1Detune.setBounds(262, 102, 56, 60);
    osc1Level.setBounds(22, 218, 56, 60);
    osc2Oct.setBounds(82, 218, 56, 60);
    osc2Semi.setBounds(142, 218, 56, 60);
    osc2Unison.setBounds(202, 218, 56, 60);
    osc2Level.setBounds(262, 218, 56, 60);

    subLevel.setBounds(884, 102, 82, 68);
    entropyAmt.setBounds(1018, 102, 82, 68);
    gravityAmt.setBounds(884, 224, 82, 68);
    ghostVoxAmt.setBounds(1018, 224, 82, 68);

    lfoTargetBox.setBounds(28, 42, 136, 24);
    lfoShapeBox.setBounds(172, 42, 136, 24);
    lfoRate.setBounds(32, 102, 74, 64);
    lfoDepth.setBounds(132, 102, 74, 64);
    pitchPunch.setBounds(232, 102, 74, 64);
    pumpAmt.setBounds(32, 218, 74, 64);
    noiseLevel.setBounds(132, 218, 74, 64);
    vhsDrift.setBounds(232, 218, 74, 64);

    arpModeBox.setBounds(850, 42, 282, 24);
    arpRate.setBounds(952, 98, 82, 62);
    stepMatrix.setBounds(852, 206, 278, 66);

    xyPad.setBounds(28, 46, 282, 330);

    distBtn.setBounds(26, 434, 72, 22);
    distModeBox.setBounds(102, 434, 148, 22);
    distDrive.setBounds(44, 486, 66, 60);
    distMix.setBounds(160, 486, 66, 60);
    crushAmount.setBounds(44, 582, 66, 60);
    lowCutHz.setBounds(160, 582, 66, 60);

    chorusBtn.setBounds(280, 434, 100, 22);
    chorusMix.setBounds(276, 480, 56, 54);
    chorusRate.setBounds(336, 480, 56, 54);
    chorusDepth.setBounds(396, 480, 56, 54);
    doublerBtn.setBounds(280, 540, 100, 22);
    doublerWidth.setBounds(276, 586, 56, 54);
    doublerDtune.setBounds(336, 586, 56, 54);
    doublerMix.setBounds(396, 586, 56, 54);

    delayBtn.setBounds(476, 434, 78, 22);
    delayModeBox.setBounds(558, 434, 146, 22);
    delayTime.setBounds(498, 486, 66, 60);
    delayFb.setBounds(614, 486, 66, 60);
    delayMix.setBounds(556, 582, 66, 60);

    reverbBtn.setBounds(736, 434, 90, 22);
    freezeBtn.setBounds(894, 434, 88, 22);
    reverbSize.setBounds(768, 486, 66, 60);
    reverbMix.setBounds(886, 486, 66, 60);
    haloAmt.setBounds(827, 582, 66, 60);

    masterGain.setBounds(1018, 464, 76, 72);
    tapeStopBtn.setBounds(1016, 602, 114, 34);

    if (showKeyboard)
        midiKeyboard.setBounds(16, 678, 1128, 68);
}
