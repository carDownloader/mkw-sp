#include "TimeAttackRulesPage.h"

#include "SectionManager.h"

#include "page/CharacterSelectPage.h"

#include <stdio.h>

static const Page_vt s_TimeAttackRulesPage_vt;

static void onBack(InputHandler *this, u32 localPlayerId) {
    UNUSED(localPlayerId);

    TimeAttackRulesPage *page = container_of(this, TimeAttackRulesPage, onBack);
    page->replacement = 0x69; // TODO enum
    Page_startReplace(page, PAGE_ANIMATION_PREV, 0.0f);
}

static const InputHandler_vt onBack_vt = {
    .handle = onBack,
};

static void onRuleControlFront(RadioButtonControlHandler *this, RadioButtonControl *control, u32 localPlayerId, s32 selected) {
    UNUSED(localPlayerId);
    UNUSED(selected); // TODO use

    TimeAttackRulesPage *page = container_of(this, TimeAttackRulesPage, onRuleControlFront);
    if (control->index < ARRAY_SIZE(page->ruleControls) - 1) {
        RadioButtonControl_select(&page->ruleControls[control->index + 1], 0);
    } else {
        PushButton_select(&page->okButton, 0);
    }
}

static const RadioButtonControlHandler_vt onRuleControlFront_vt = {
    .handle = onRuleControlFront,
};

static void onRuleControlSelect(RadioButtonControlHandler *this, RadioButtonControl *control, u32 localPlayerId, s32 selected) {
    UNUSED(localPlayerId);

    if (selected < 0) {
        return;
    }

    TimeAttackRulesPage *page = container_of(this, TimeAttackRulesPage, onRuleControlSelect);
    u32 messageIds[][3] = {
        { 0xd58, 0xd5a },
        { 0x3012, 0x3013, 0x3014 },
        { 0x3016, 0x3017, 0x3018 },
    };
    u32 messageId = messageIds[control->index][selected];
    CtrlMenuInstructionText_setMessage(&page->instructionText, messageId, NULL);
}

static const RadioButtonControlHandler_vt onRuleControlSelect_vt = {
    .handle = onRuleControlSelect,
};

static void onOkButtonFront(PushButtonHandler *this, PushButton *button, u32 localPlayerId) {
    UNUSED(button);
    UNUSED(localPlayerId);

    TimeAttackRulesPage *page = container_of(this, TimeAttackRulesPage, onOkButtonFront);
    page->replacement = PAGE_ID_CHARACTER_SELECT;
    Section *currentSection = s_sectionManager->currentSection;
    CharacterSelectPage *characterSelectPage = (CharacterSelectPage *)currentSection->pages[PAGE_ID_CHARACTER_SELECT];
    characterSelectPage->prevId = page->id;
    // TODO delay
    Page_startReplace(page, PAGE_ANIMATION_NEXT, 0.0f);
}

static const PushButtonHandler_vt onOkButtonFront_vt = {
    .handle = onOkButtonFront,
};

static void onOkButtonSelect(PushButtonHandler *this, PushButton *button, u32 localPlayerId) {
    UNUSED(button);
    UNUSED(localPlayerId);

    TimeAttackRulesPage *page = container_of(this, TimeAttackRulesPage, onOkButtonSelect);
    CtrlMenuInstructionText_setMessage(&page->instructionText, 0x3019, NULL);
}

static const PushButtonHandler_vt onOkButtonSelect_vt = {
    .handle = onOkButtonSelect,
};

extern void ClassSelectPage_ct;

static TimeAttackRulesPage *TimeAttackRulesPage_ct(TimeAttackRulesPage *this) {
    Page_ct(this);
    this->vt = &s_TimeAttackRulesPage_vt;

    MultiControlInputManager_ct(&this->inputManager);
    CtrlMenuPageTitleText_ct(&this->pageTitleText);
    PushButton_ct(&this->okButton);
    CtrlMenuInstructionText_ct(&this->instructionText);
    CtrlMenuBackButton_ct(&this->backButton);
    for (u32 i = 0; i < ARRAY_SIZE(this->ruleControls); i++) {
        RadioButtonControl_ct(&this->ruleControls[i]);
    }
    this->onBack.vt = &onBack_vt;
    this->onRuleControlFront.vt = &onRuleControlFront_vt;
    this->onRuleControlSelect.vt = &onRuleControlSelect_vt;
    this->onOkButtonFront.vt = &onOkButtonFront_vt;
    this->onOkButtonSelect.vt = &onOkButtonSelect_vt;

    return this;
}
PATCH_B(ClassSelectPage_ct, TimeAttackRulesPage_ct);

static void TimeAttackRulesPage_dt(Page *base, s32 type) {
    TimeAttackRulesPage *this = (TimeAttackRulesPage *)base;

    for (u32 i = ARRAY_SIZE(this->ruleControls); i --> 0;) {
        RadioButtonControl_dt(&this->ruleControls[i], -1);
    }
    CtrlMenuBackButton_dt(&this->backButton, -1);
    CtrlMenuInstructionText_dt(&this->instructionText, -1);
    PushButton_dt(&this->okButton, -1);
    CtrlMenuPageTitleText_dt(&this->pageTitleText, -1);
    MultiControlInputManager_dt(&this->inputManager, -1);

    Page_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

static s32 TimeAttackRulesPage_getReplacement(Page *base) {
    TimeAttackRulesPage *this = (TimeAttackRulesPage *)base;

    return this->replacement;
}

static void TimeAttackRulesPage_onInit(Page *base) {
    TimeAttackRulesPage *this = (TimeAttackRulesPage *)base;

    MultiControlInputManager_init(&this->inputManager, 0x1, false);
    this->baseInputManager = &this->inputManager;
    MultiControlInputManager_setPointerMode(&this->inputManager, 0x1);

    Page_initChildren(this, 4 + ARRAY_SIZE(this->ruleControls));
    Page_insertChild(this, 0, &this->pageTitleText, 0);
    Page_insertChild(this, 1, &this->okButton, 0);
    Page_insertChild(this, 2, &this->instructionText, 0);
    Page_insertChild(this, 3, &this->backButton, 0);
    for (u32 i = 0; i < ARRAY_SIZE(this->ruleControls); i++) {
        Page_insertChild(this, 4 + i, &this->ruleControls[i], 0);
    }

    CtrlMenuPageTitleText_load(&this->pageTitleText, false);
    PushButton_load(&this->okButton, "button", "VSSetting", "ButtonOK", 0x1, false, false);
    CtrlMenuInstructionText_load(&this->instructionText);
    PushButton_load(&this->backButton, "button", "Back", "ButtonBack", 0x1, false, true);
    const char *ruleNames[] = {
        "Class",
        "GhostTags",
        "SolidGhosts",
    };
    u32 buttonCounts[] = { 2, 3, 3 };
    for (u32 i = 0; i < ARRAY_SIZE(this->ruleControls); i++) {
        char variant[0x20];
        snprintf(variant, sizeof(variant), "Radio%s", ruleNames[i]);
        char buffers[3][0x20];
        const char *buttonVariants[3];
        for (u32 j = 0; j < buttonCounts[i]; j++) {
            snprintf(buffers[j], sizeof(*buffers), "Option%s%lu", ruleNames[i], j);
            buttonVariants[j] = buffers[j];
        }
        RadioButtonControl_load(&this->ruleControls[i], buttonCounts[i], 0, "control", "TASettingRadioBase", variant, "TASettingRadioOption", buttonVariants, 0x1, false, false);
        this->ruleControls[i].index = i;
    }

    MultiControlInputManager_setHandler(&this->inputManager, INPUT_ID_BACK, &this->onBack, false, false);
    for (u32 i = 0; i < ARRAY_SIZE(this->ruleControls); i++) {
        RadioButtonControl *control = &this->ruleControls[i];
        RadioButtonControl_setFrontHandler(control, &this->onRuleControlFront);
        RadioButtonControl_setSelectHandler(control, &this->onRuleControlSelect);
    }
    PushButton_setFrontHandler(&this->okButton, &this->onOkButtonFront, false);
    PushButton_setSelectHandler(&this->okButton, &this->onOkButtonSelect);

    CtrlMenuPageTitleText_setMessage(&this->pageTitleText, 0xd48, NULL);
}

static void TimeAttackRulesPage_onActivate(Page *base) {
    TimeAttackRulesPage *this = (TimeAttackRulesPage *)base;

    PushButton_selectDefault(&this->okButton, 0);
    CtrlMenuInstructionText_setMessage(&this->instructionText, 0x3019, NULL);

    this->replacement = -1; // TODO enum
}

static const Page_vt s_TimeAttackRulesPage_vt = {
    .dt = TimeAttackRulesPage_dt,
    .vf_0c = &Page_vf_0c,
    .getReplacement = TimeAttackRulesPage_getReplacement,
    .vf_14 = &Page_vf_14,
    .vf_18 = &Page_vf_18,
    .changeSection = Page_changeSection,
    .vf_20 = &Page_vf_20,
    .push = &Page_push,
    .onInit = TimeAttackRulesPage_onInit,
    .vf_2c = &Page_vf_2c,
    .onActivate = TimeAttackRulesPage_onActivate,
    .vf_34 = &Page_vf_34,
    .vf_38 = &Page_vf_38,
    .vf_3c = &Page_vf_3c,
    .vf_40 = &Page_vf_40,
    .vf_44 = &Page_vf_44,
    .vf_48 = &Page_vf_48,
    .vf_4c = &Page_vf_4c,
    .vf_50 = &Page_vf_50,
    .onRefocus = Page_onRefocus,
    .vf_58 = &Page_vf_58,
    .vf_5c = &Page_vf_5c,
    .vf_60 = &Page_vf_60,
};